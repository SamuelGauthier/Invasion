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
	extern GCEntry* pHead;
	
	void init();
	void add(release_function func, void* arg);
	void release_all();
} /* GarbageCollector */

//------------------------------------------------
// Instance
//------------------------------------------------
struct Instance
{
	Vec3f pos;
	Qua4f rot;
	Vec3f dir;
	Vec3f dest;
	Uint8 id;
	int state;
	bool oriented;

	Instance* pNext;
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
		transparent_textures = (1<<2),
		complete_transparent = (1<<3)
	};

	void* mesh;
	void (*render_function)(void*, float);
	void (*set_animation)(void*, int);
	Vec3f trans, scale;
	Qua4f rot;
	Vec3f dir;
	Instance* pProList;
	int type;

	Entity* pNext;
};

//------------------------------------------------
// Picking
//------------------------------------------------
namespace Picking
{
	void point(Entity* list, Entity* e, Instance* i);
};

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
	extern Entity* pESelect;
	extern Instance* pISelect;
	extern Entity* pEntityList;
	static Text::INV_Font* fontInter = NULL;
	static Uint8 id_head = 100;

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
	
		glClearColor(0.15f, 0.15f, 0.15f, 1.f);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(75.0, (GLdouble)width/(GLdouble)height, 0.1, 100000.0);
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
	Entity* addMesh(void* mesh, void (*render_function)(void*, float), void (*set_animation)(void*, int), int type, Vec3f trans, Vec3f scale, Vec3f dir);
	void goTo(Instance* i, float x, float y, float z);
	void createInstance(Entity* e, Vec3f pos, Vec3f dir);
	void new_entity(char* arg[], void*);
} /* Game */

//------------------------------------------------
// Model Manager
//------------------------------------------------
namespace ModelManager 
{
	struct ModelProp 
	{
		char name[16];
		ModelProp* pNext;
	}; /* ModelProp */
	
	extern ModelProp* pModelList;
	void load();
	void write(FILE* base, ModelProp* mp);
	char* getList();
	ModelProp* read(FILE* base);
	void unload(void*);
	ModelProp* search(char* name);
	void remove(char* name);
} /* ModelManager */

#endif
