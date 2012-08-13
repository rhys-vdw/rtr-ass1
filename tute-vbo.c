/* tute-vbo.c: Sat Jul 21 23:04:33 EST 2012 pknowles */

#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

/* sdl-base contains opengl/SDL init code and provides
 * a main loop. this file implements expected callback
 * functions. see sdl-base.h */
#include "sdlbase.h"

#include "vec3f.h"
#include "vertex.h"
#include "mesh_generation.h"

void generateSphereTriangleVertexArray(float radius, int slices, int stacks);
void generateSphereTriangleStripVertexArray(float radius, int slices, int stacks);

/* Basic camera struct */
typedef struct {
	int rotating;
	int zooming;
	float rotX, rotY;
	float zoom;
	float sensitivity;
} Camera;

/* Render states */
enum RenderOptions {
	RENDER_LIGHTING,
	RENDER_WIREFRAME,
	RENDER_FLAT,
	RENDER_CULL_BACKFACE,
	NUM_RENDER_OPTIONS /* MUST BE LAST! */
};

/* Scene globals */
Camera camera;
float currentFramerate;
static float deltaTime;
int windowWidth;
int windowHeight;
int lastMouseX = 0;
int lastMouseY = 0;
int tesselation = 16;

Vertex *vertices = NULL;
int vertexCount = 0;

int *indices = NULL;
int indexCount = 0;

int meshType = PLANE;

GLuint vertexBufferId = 0;

bool renderOptions[NUM_RENDER_OPTIONS] = { false, true, true, false };

/* Update opengl state to match flags in renderOptions */
void setRenderOptions()
{
	if (renderOptions[RENDER_LIGHTING])
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	if (renderOptions[RENDER_CULL_BACKFACE])
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	glShadeModel(renderOptions[RENDER_FLAT] ? GL_FLAT : GL_SMOOTH);

	glPolygonMode(GL_FRONT_AND_BACK,
		renderOptions[RENDER_WIREFRAME] ? GL_LINE : GL_FILL);
}

/* Generate the selected mesh at a given tesselation and load it into the mesh
 * VBO as well as storing it into an array to be drawn in immediate mode. */
void generateMesh () {
	/* generate vertices */
	int slices, stacks;

	switch (meshType) {
		case SPHERE:
			slices = tesselation * 2;
			stacks = tesselation;
			vertexCount = generateSphereVertices(&vertices, 1.0f, slices, stacks);
			break;
		case TORUS:
			slices = tesselation * 2;
			stacks = tesselation;
			vertexCount = generateTorusVertices(&vertices, 1.0f, 0.5f, slices,
					stacks);
			break;
		case PLANE:
			slices = stacks = tesselation;
			vertexCount = generatePlaneVertices(&vertices, 1.0f, slices, stacks);
			break;
		default:
			assert(false);
			break;
	}
	indexCount = generateIndices(&indices, slices, stacks);

	/* load vertices into VBO */
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_READ);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* Called once at program start */
void init()
{
	glewInit();

	int argc = 0;  /* fake glutInit args */
	char *argv = "";
	glutInit(&argc, &argv);

	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);

	memset(&renderOptions, 0, sizeof(renderOptions));
	renderOptions[RENDER_LIGHTING] = true;
	memset(&camera, 0, sizeof(Camera));
	camera.sensitivity = 0.3f;
	camera.zoom = 2.0f;

	/* create vertex buffer */
	glGenBuffers(1, &vertexBufferId);

	/* generate the first mesh */
	generateMesh();
}

/* Called once at start and again on window resize */
void reshape(int width, int height)
{
	windowWidth = width;
	windowHeight = height;

	/* Portion of viewport to render to */
	glViewport(0, 0, width, height);

	/* Calc aspect ratio */
	float aspect = width / (float)height;

	/* Begin editing projection matrix */
	glMatrixMode(GL_PROJECTION);

	/* Clear previous projection */
	glLoadIdentity();

	/* Generate perspective projection matrix */
	gluPerspective(75.0f, aspect, 0.01f, 100.0f);

	/* Restore modelview as current matrix */
	glMatrixMode(GL_MODELVIEW);
}


void drawTrianglesFromVbo() {
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glInterleavedArrays(GL_N3F_V3F, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, indices);
}

void drawTrianglesFromArray() {
	int i;
	float t = 0.0f;

	/* iterate through array */
	glBegin(GL_TRIANGLE_STRIP);
	for (i = 0; i < vertexCount; i++) {
		t = ((float) i / (float) vertexCount);
		glColor3f(t, 1.0f - t, 0.0f);
		glNormal3f(vertices[i].normal.x, vertices[i].normal.y, vertices[i].normal.z);
		glVertex3f(vertices[i].pos.x , vertices[i].pos.y, vertices[i].pos.z);
	}
	glEnd();
}

void drawAxes(float size) {
	/* Backup previous "enable" state */
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(size, 0.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, size, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, size);

	glEnd();

	/* Restore "enable" state */
	glPopAttrib();
}

void drawOSD()
{
	char *bufp;
	char buffer[32];

	/* Backup previous "enable" state */
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	/* Create a temporary orthographic projection, matching
	 * window dimensions, and push it onto the stack */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	/* draw framerate */
	glPushMatrix();
	glLoadIdentity(); /* clear current modelview (ie. from display) */
	snprintf(buffer, sizeof(buffer), "FR(fps): %d delta: %f", (int)currentFramerate, deltaTime);
	glRasterPos2i(10, 10);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();  /* pop projection */
	glMatrixMode(GL_MODELVIEW);

	/* Restore "enable" state */
	glPopAttrib();
}

void display()
{
	static float lightPosition[] = {1.0f, 1.0f, 1.0f, 0.0f};
	static float lightAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f}; /* Constant, add to everything */
	static float materialDiffuse[] = {0.7f, 0.7f, 0.7f, 1.0f}; /* Brighter as surface faces light */
	static float materialSpecular[] = {0.3f, 0.3f, 0.3f, 1.0f}; /* Highlight, direct reflection from light */
	static float materialShininess = 64.0f; /* 1 to 128, higher gives sharper highlight */

	/* Clear the colour and depth buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Camera transformations */
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -camera.zoom);
	glRotatef(camera.rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(camera.rotY, 0.0f, 1.0f, 0.0f);

	/* Draw scene */
	setRenderOptions();
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);

	drawTrianglesFromVbo();
	drawAxes(1.0f);

	/* OSD - framerate etc */
	drawOSD();
}

/* Called continuously. dt is time between frames in seconds */
void update(float dt)
{
	deltaTime = dt * 1000;
	static float fpsTime = 0.0f;
	static int fpsFrames = 0;
	fpsTime += dt;
	fpsFrames += 1;
	if (fpsTime > 1.0f)
	{
		currentFramerate = fpsFrames / fpsTime;
		fpsTime = 0.0f;
		fpsFrames = 0;
	}
}

void mouseDown(int button, int state, int x, int y)
{
	if (button == SDL_BUTTON_LEFT)
		camera.rotating = (state == 1);
	if (button == SDL_BUTTON_RIGHT)
		camera.zooming = (state == 1);
}

void mouseMove(int x, int y)
{
	int dx = x - lastMouseX;
	int dy = y - lastMouseY;
	if (camera.rotating)
	{
		camera.rotY += dx * camera.sensitivity;
		camera.rotX += dy * camera.sensitivity;
	}
	if (camera.zooming)
	{
		camera.zoom -= dy * camera.zoom * camera.sensitivity * 0.03f;
	}
	lastMouseX = x;
	lastMouseY = y;
}

/* Change to another mesh */
void changeMeshType (int change) {
	if (change != 0) {
		meshType = (meshType + change) % NUM_MESH_TYPES;
		if (meshType < 0) {
			meshType = NUM_MESH_TYPES - fabs(meshType);
		}
		generateMesh();
	}
}

void changeTesselation (int delta) {
	tesselation += delta;
	generateMesh();
}

void keyDown(int key)
{
	if (key == SDLK_ESCAPE) {
		quit();
	} else if (key == SDLK_F3) {
		renderOptions[RENDER_LIGHTING] = !renderOptions[RENDER_LIGHTING];
	} else if (key == SDLK_F4) {
		renderOptions[RENDER_WIREFRAME] = !renderOptions[RENDER_WIREFRAME];
	} else if (key == SDLK_F1) {
		changeTesselation(-tesselation / 2);
	} else if (key == SDLK_F2) {
		changeTesselation(tesselation);
	} else if (key == SDLK_q) {
		changeMeshType(1);
	} else if (key == SDLK_a) {
		changeMeshType(-1);
	}
}

void keyUp(int key)
{
}

void cleanup()
{
}

