#include <time.h>

#include "sdlbase.h"

/* default SDL window arguments */
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_DEPTH 32
#define DEFAULT_FLAGS (SDL_OPENGL | SDL_RESIZABLE)

#define FPS_CAP 30
#define MS_TO_NS 1000000
#define S_TO_NS 1000000000 //a second to nanosecond conversion

static int quitFlag = 0;
static int idleFlag = 0;

void toggleIdle()
{
	idleFlag = !idleFlag;
}

void quit()
{
	/* stop running main loop */
	quitFlag = 1;
}

void eventReshape(int w, int h)
{
	/* update viewport and projection - define in client app */
	reshape(w, h);
}

void processEvents()
{
	static int lastMouseX = 0;
	static int lastMouseY = 0;
	SDL_Event event;
	SDL_Surface *screen;
	int videoFlags = DEFAULT_FLAGS;

	/* route all events to specific event handlers */
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			quit();
			break;
		case SDL_VIDEORESIZE:
			screen = SDL_SetVideoMode(
				event.resize.w, 
				event.resize.h,
				DEFAULT_DEPTH,
				videoFlags
				);
			eventReshape(screen->w, screen->h);
			break;
		case SDL_KEYDOWN:
			keyDown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			keyUp(event.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			mouseDown(event.button.button, 1, lastMouseX, lastMouseY);
			break;
		case SDL_MOUSEBUTTONUP:
			mouseDown(event.button.button, 0, lastMouseX, lastMouseY);
			break;
		case SDL_MOUSEMOTION:
			lastMouseX = event.motion.x;
			lastMouseY = event.motion.y;
			mouseMove(lastMouseX, lastMouseY);
			break;
		default:
			break;
		}
	}
}

//this is the actual function
void idle(unsigned int delta)
{
	struct timespec fTime;// this is an existing struct
	float nanoDelta = MS_TO_NS * delta;
	fTime.tv_sec = 0.0f;
	fTime.tv_nsec = (S_TO_NS / FPS_CAP) - nanoDelta;
//so a nanosecond / requiredFR minus the time other stuff took

	nanosleep(&fTime, NULL);
}

int main(int argc, char** argv)
{
	SDL_Surface *screen;
	int videoFlags = DEFAULT_FLAGS;
	unsigned int lastTime, thisTime, deltaTime;

	/* initialize sdl and opengl */
	videoFlags = DEFAULT_FLAGS;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	screen = SDL_SetVideoMode(
		DEFAULT_WIDTH,
		DEFAULT_HEIGHT,
		DEFAULT_DEPTH,
		videoFlags
		);
	glEnable(GL_DEPTH_TEST);

	/* initialize the sdl app */
	init();
	eventReshape(screen->w, screen->h);
	
	glClear(GL_ACCUM_BUFFER_BIT);
		
	/* the main loop */
	lastTime = SDL_GetTicks();
	while (!quitFlag)
	{
		/* calculate elapsed time */
		thisTime = SDL_GetTicks();
		deltaTime = thisTime - lastTime;
		lastTime = thisTime;

		/* read sdl events */
		processEvents();

		/* cpu-side logic, movement/animation etc */
		update((float)deltaTime * 0.001);

		/* render results */
		display();

		SDL_GL_SwapBuffers();

		/* delay to maintain framerate */
		if (idleFlag) {
			idle(SDL_GetTicks() - thisTime);
		}
	}

	cleanup();
	return 0;
}
