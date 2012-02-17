#ifndef INVBASE_H
#define INVBASE_H
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

//------------------------------------------------
// Garbage collector
//------------------------------------------------
namespace GarbageCollector 
{
	typedef void (*release_function)(void*);
	struct GCEntry
	{
		release_function func;
		void* arg;
		GCEntry* pNext;
	};
	static GCEntry* pHead = NULL;
	
	void init();
	void add(release_function func, void* arg);
	inline void release_all()
	{
		GCEntry* tmp;
		while(pHead)
		{
			tmp = pHead;
			pHead = pHead->pNext;
			
			(*tmp->func)(tmp->arg);
			delete tmp;
		}
	}
} /* GarbageCollector */

//------------------------------------------------
// Property
//------------------------------------------------
struct Property
{
	Vec3f* pos;
	Property* pNext;
};

//------------------------------------------------
// Entity
//------------------------------------------------
struct Entity
{
	enum ENTITY_STATE
	{
		stand = 1,
		walking,
	};

	enum ENTITY_TYPE
	{
		animation = (1<<0),
		mobile = (1<<1),
		transparent_textures = (1<<2)
	};

	void* mesh;
	void (*render_function)(void*, float);
	void (*set_animation)(void*, int);
	Vec3f trans, scale, dest;
	Qua4f rot;
	float angleX, angleY, angleZ;
	Property* pProList;
	Uint8 id;
	bool selected;
	bool oriented;
	int state;
	int type;

	Entity* pNext;
};

//------------------------------------------------
// Scenery
//------------------------------------------------
namespace Scenery 
{
	static float angle = 0.f;
	void drawCube(float x, float y, float z);
} /* Scenery */

//------------------------------------------------
// Game
//------------------------------------------------
namespace Game 
{
	enum BIND_TYPE
	{
		INPUT_FUNCTION = 1,
		STOP_WHEN_FUNCTION,
		MOTION_FUNCTION,
	};

	extern bool texture;
	extern int width;
	extern int height;
	extern int fps;
	extern Entity* pSelect;
	static Entity* pEntityList = NULL;
	static Text::INV_Font* fontInter = NULL;
	static Uint8 id_head = 1;

	typedef bool (*event_function)();
	static event_function stopwhen;
	typedef void (*tick_function)(float timeElapsed);
	static tick_function tick;
	typedef void (*input_function)(void);
	static input_function input;
	typedef void (*init_function)(void);
	static init_function initInter = 0;
	typedef void (*write_function)(Text::INV_Font*);
	static write_function renderInter;
	static input_function inputInter;
	typedef void (*void_function)(void);
	static void_function lookCamera;
	static tick_function inputCamera;
	static tick_function motion;

	inline bool init(int w=640, int h=480, bool fullscreen = false, int frame_per_second = 60, int bpp=32)
	{

		SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
		atexit(SDL_Quit);

		// Set GL attributes
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		
		if(!SDL_SetVideoMode(w, h, bpp, SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN:0))){
			fprintf(stderr, "Could not set GL mode : %s", SDL_GetError());
			return false;
		}

		width = w;
		height = h;
		fps = frame_per_second;
	
		glClearColor(0.f, 0.f, 0.0f, 1.f);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0, (GLdouble)width/(GLdouble)height, 1.0, 100000.0);
		glEnable(GL_DEPTH_TEST);

		return true;
	}


	inline void setTitle(const char* title)
	{
		SDL_WM_SetCaption(title, NULL);
	}

	void bind(BIND_TYPE type, void* func);
	void mainLoop();
	bool isPressed();
	void exit(char**, void*);

	void bindInterface(init_function it, write_function wt, input_function ip, Text::INV_Font* font);
	void bindCamera(void_function lookCam, tick_function inputCam);
	Entity* addMesh(void* mesh, void (*render_function)(void*, float), void (*set_animation)(void*, int), int type, Vec3f trans, Vec3f scale, float angleX, float angleY, float angleZ);
	void goTo(Entity* e, float x, float y, float z);
	Entity* getSelected();
} /* Game */

#endif
