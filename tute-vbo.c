/* tute-vbo.c: Sat Jul 21 23:04:33 EST 2012 pknowles */

#include <math.h>
#include <string.h>
#include <stdbool.h>

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

/* Render state enums */
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

	printf("generate\n");
	//generateSphereTriangleVertexArray(1.0f, tesselation * 2, tesselation)kkkjjN
	//vertexCount = generateTorusVertices(&vertices, 1.0f, 0.5f, tesselation * 2, tesselation);
	vertexCount = generatePlaneVertices(&vertices, 1.0f, tesselation);
	printf("done\n");

	/* create vertex buffer */
	glGenBuffers(1, &vertexBufferId);

	printf("vertexCount = %d\n", vertexCount);

	/* put array into buffer */
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_READ);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

#if 0
void drawSphereVertex(float r, float u, float v)
{
	/* Use maths rather than physics spherical coordinate convention */
	float theta = u * 2.0 * pi;
	float phi = v * pi;

	float x = r * cos(theta) * sin(phi);
	float y = r * sin(theta) * sin(phi);
	float z = r * cos(phi);

	glNormal3f(x/r, y/r, z/r);
	glVertex3f(x, y, z);
}

Vertex getSphereVertex(float r, float u, float v) {
	Vertex vertex;

	/* Use maths rather than physics spherical coordinate convention */
	float theta = u * 2.0 * pi;
	float phi = v * pi;

	float x = r * cos(theta) * sin(phi);
	float y = r * sin(theta) * sin(phi);
	float z = r * cos(phi);
	
	vertex.normal.x = x / r;
	vertex.normal.y = y / r;
	vertex.normal.z = z / r;

	vertex.pos.x = x;
	vertex.pos.y = y;
	vertex.pos.z = z;

	return vertex;
}

Vertex getTorusVertex(float R, float r, float u, float v) {
	Vertex vertex;

	/* Use maths rather than physics spherical coordinate convention */
	float theta = u * 2.0f * pi;
	float phi = v * 2.0f * pi;

	float x = (R + r * sin(theta)) * cos(phi);
	float y = (R + r * sin(theta)) * sin(phi);
	float z = r * cos(phi);
	
	vertex.normal.x = r * sin(theta) * cos(phi);
	vertex.normal.y = r * sin(theta) * sin(phi);
	vertex.normal.z = cos(phi);

	vertex.pos.x = x;
	vertex.pos.y = y;
	vertex.pos.z = z;

	return vertex;
}

void generateTorusTriangleVertexArray(float major, float minor, int slices, int stacks) {
	int i, j;
	float u, v, u1, v1;

	/* Grid divisions (dimensions of quads, "fences") */
	int divsU = slices;
	int divsV = stacks;
	
	/* Vertex dimensions ("posts"). Note for a sphere, the
	 * last vertex of each row is the same as the first */
	int rows = divsU + 1;
	int cols = divsV + 1;

	/* update array size - two triangles per segment */
	vertexCount = slices * stacks * 6;
	vertices = (Vertex *) realloc(vertices, vertexCount * sizeof(Vertex));

	/* j outer loop over i inner loop for each circle */
	for (j = 0; j < cols - 1; ++j) {
		v = j / (float)(cols - 1);
		v1 = (j + 1) / (float)(cols - 1);
		for (i = 0; i < rows - 1; ++i) {
			u = i / (float)(rows - 1);
			u1 = (i + 1) / (float)(rows - 1);

			int index = ((j * divsU) + i) * 6;
			vertices[index + 0] = getTorusVertex(major, minor, u, v);
			vertices[index + 1] = getTorusVertex(major, minor, u, v1);
			vertices[index + 2] = getTorusVertex(major, minor, u1, v1);

			vertices[index + 3] = getTorusVertex(major, minor, u, v);
			vertices[index + 4] = getTorusVertex(major, minor, u1, v1);
			vertices[index + 5] = getTorusVertex(major, minor, u1, v);
		}
	}
}

void generateSphereTriangleVertexArray(float radius, int slices, int stacks) {
	int i, j;
	float u, v, u1, v1;

	/* Grid divisions (dimensions of quads, "fences") */
	int divsU = slices;
	int divsV = stacks;
	
	/* Vertex dimensions ("posts"). Note for a sphere, the
	 * last vertex of each row is the same as the first */
	int rows = divsU + 1;
	int cols = divsV + 1;

	/* update array size - two triangles per segment */
	vertexCount = slices * stacks * 6;
	vertices = (Vertex *) realloc(vertices, vertexCount * sizeof(Vertex));

	/* j outer loop over i inner loop for each circle */
	for (j = 0; j < cols - 1; ++j) {
		v = j / (float)(cols - 1);
		v1 = (j + 1) / (float)(cols - 1);
		for (i = 0; i < rows - 1; ++i) {
			u = i / (float)(rows - 1);
			u1 = (i + 1) / (float)(rows - 1);

			int index = ((j * divsU) + i) * 6;
			vertices[index + 0] = getSphereVertex(radius, u, v);
			vertices[index + 1] = getSphereVertex(radius, u, v1);
			vertices[index + 2] = getSphereVertex(radius, u1, v1);

			vertices[index + 3] = getSphereVertex(radius, u, v);
			vertices[index + 4] = getSphereVertex(radius, u1, v1);
			vertices[index + 5] = getSphereVertex(radius, u1, v);
		}
	}
}
#endif

void drawTrianglesFromVbo() {
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glInterleavedArrays(GL_N3F_V3F, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
}

void drawTrianglesFromArray()
{
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

#if 0
void drawSphere(float radius, int slices, int stacks)
{
	int i, j;
	float u, v, u1, v1;

	/* Be carefull of loop and indices - 
	 * http://en.wikipedia.org/wiki/Off-by-one_error */
	
	/* Grid divisions (dimensions of quads, "fences") */
	int divsU = slices;
	int divsV = stacks;
	
	/* Vertex dimensions ("posts"). Note for a sphere, the
	 * last vertex of each row is the same as the first */
	int rows = divsU + 1;
	int cols = divsV + 1;

	/* j outer loop over i inner loop for each circle */
	glBegin(GL_TRIANGLES);
	for (j = 0; j < cols - 1; ++j)
	{
		v = j / (float)(cols - 1);
		v1 = (j + 1) / (float)(cols - 1);
		for (i = 0; i < rows - 1; ++i)
		{
			u = i / (float)(rows - 1);
			u1 = (i + 1) / (float)(rows - 1);
			drawSphereVertex(radius, u, v);
			drawSphereVertex(radius, u, v1);
			drawSphereVertex(radius, u1, v1);
			
			drawSphereVertex(radius, u, v);
			drawSphereVertex(radius, u1, v1);
			drawSphereVertex(radius, u1, v);
		}
	}
	glEnd();
}

void drawSphereStrips(float radius, int slices, int stacks)
{
	int i, j;
	float u, v, u1, v1;

	/* Grid divisions (dimensions of quads, "fences") */
	int divsU = slices;
	int divsV = stacks;
	
	/* Vertex dimensions ("posts"). Note for a sphere, the
	 * last vertex of each row is the same as the first */
	int rows = divsU + 1;
	int cols = divsV + 1;

	/* j outer loop over i inner loop for each circle */
	glBegin(GL_TRIANGLE_STRIP);
	for (j = 0; j < cols - 1; ++j)
	{
		v = j / (float)(cols - 1);
		v1 = (j + 1) / (float)(cols - 1);
		for (i = 0; i < rows - 1; ++i)
		{
			u = i / (float)(rows - 1);
			u1 = (i + 1) / (float)(rows - 1);

			glColor3f(v, u, 0.0f);

			drawSphereVertex(radius, u, v);
			drawSphereVertex(radius, u, v1);
			
			drawSphereVertex(radius, u1, v);
			drawSphereVertex(radius, u1, v1);
		}
	}
	glEnd();
}
#endif

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

	//drawTrianglesFromArray();
	drawTrianglesFromVbo();
	//drawSphere(1.0f, tesselation * 2, tesselation);
	//drawSphereStrips(1.0f, tesselation * 2, tesselation);
	drawAxes(1.0f);
	//glutSolidSphere(1.0f, 8, 4);

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

void changeTesselation (int delta) {
	tesselation += delta;
	printf("tesselation is now %d\n", tesselation);
	//generateSphereTriangleVertexArray(1.0f, tesselation * 2, tesselation);
}

void keyDown(int key)
{
	if (key == SDLK_ESCAPE)
		quit();
	if (key == SDLK_F3)
		renderOptions[RENDER_LIGHTING] = !renderOptions[RENDER_LIGHTING];
	if (key == SDLK_F4)
		renderOptions[RENDER_WIREFRAME] = !renderOptions[RENDER_WIREFRAME];
	if (key == SDLK_F1)
		changeTesselation(-tesselation / 2);
	if (key == SDLK_F2)
		changeTesselation(tesselation);
}

void keyUp(int key)
{
}

void cleanup()
{
}

