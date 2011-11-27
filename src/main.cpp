#define FPS 60.f
#define WIDTH_WINDOW 800
#define HEIGHT_WINDOW 600

// Créer un objet buffer /////
#include <windows.h>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include "utils.h"
#include "camera.h"
#include "objloader.h"
#include "text.h"
#include "terrain.h"
#include "ui.h"
#include "console.h"
#include "timer.h"
#include "md2loader.h"

enum
{
	CMD_NONE = 0,
	CMD_UNKNOWN,
	CMD_EXIT,
	CMD_NEW_TERRAIN,
	CMD_RELEASE_VERSION,
	CMD_GET_POS,
	CMD_GET_VIDEOMODE
};

// Variables globales
GCamera* cam = 0;
GOBJModel *tonneau = 0;
GConsole* cmd = 0;
GTerrain* trn = 0;

int anim_index = 0;

GTimer* t = 0;
GFont* f = 0;

bool key_state[256] = {false};

// Prototypes de fonctions
inline void OnCreate();
void OnRelease();
inline void OnRender();
inline void OnRender2D();
inline void OnLoop();
inline void OnProcessCommand();

void OnCreate(){
	// init Camera
	cam = initCamera(freefly_camera, Vec3f(0.f, 40.f, 0.f));

	// init Timer
	t = initTimer();
	srand(time(NULL));

	// init Font
	f = loadFont("Test.fnt");

	// init console
	cmd = initConsole();

	GCommand* cmd_new = newLineCommand(0, "new", 0);
	GCommand* cmd_release = newRowCommand(cmd_new, "release", 0);
	GCommand* cmd_get = newRowCommand(cmd_new, "get", 0);
	newRowCommand(cmd_new, "exit", CMD_EXIT);
	newRowCommand(cmd_new, "quit", CMD_EXIT);
	newLineCommand(cmd_new, "terrain", CMD_NEW_TERRAIN);
	
	newLineCommand(cmd_release, "version", CMD_RELEASE_VERSION);

	GCommand* cmd_get_pos = newLineCommand(cmd_get, "pos", CMD_GET_POS);
	newRowCommand(cmd_get_pos, "videomode", CMD_GET_VIDEOMODE);
	setCommandTree(cmd, cmd_new);

	// load models
	tonneau = loadOBJ("tonneau.obj");
	setTexture(tonneau, "wood.tga");

	// textures
	trn = initTerrain();

	planeTerrain(trn, 100);
	randomGen(trn, 15, 5, 100);
	setTexture(trn, "grass.tga",0);
	setTexture(trn, "concrete.tga", 1);
	trn->cell = 1.f;
	trn->tex_size = 0.125f;
	fillBuffers(trn); 
	
	// init state
	glEnable(GL_DEPTH_TEST);

	// Enable V-Sync
	wglSwapIntervalEXT(1);

	// exit
	atexit(OnRelease);

}

void OnRender(){

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	lookCamera(cam);

	glScalef(5.f, 5.f, 5.f);
	drawRepere();

	glColor3f(1.f, 1.f, 1.f);

	/*
	glPushMatrix();
	glTranslatef(3.f,1.f,0.f);
	render(tonneau);
	glTranslatef(2.6f,0.0f,2.7f);
	render(tonneau);
	glTranslatef(-3.f,0.f, 2.f);
	render(tonneau);
	glTranslatef(-4.f,0.5f,3.f);
	glRotatef(90.f,1.f,0.f,0.f);
	render(tonneau);
	glPopMatrix();
	*/
	
	render(trn);
	
	//---------------------------------
	// Render 2D
	//---------------------------------

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	glOrtho(0,800,600,0,-1,5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	OnRender2D();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//---------------------------------
	// End Render 2D
	//---------------------------------

	SDL_GL_SwapBuffers();
}

void OnRender2D()
{
	render(cmd, f);
}

void OnProcessCommand()
{
	int id = getCommand(cmd);

	if(!id)
		return;

	// On efface la ligne qui a été entré
	clearline_console(cmd);

	switch(id)
	{
		case CMD_NEW_TERRAIN:
			putline_console(cmd, "new terrain created");
			break;
		case CMD_RELEASE_VERSION:
			putline_console(cmd, "v0.0.1 pre-pre-alpha (18.10.2011)");
			break;
		case CMD_GET_POS:
			putline_console(cmd, "pos(%f, %f, %f)", cam->pos.x, cam->pos.y, cam->pos.z);
			break;
		case CMD_GET_VIDEOMODE:
			putline_console(cmd, "video mode : 800x600x32");
			break;
		case CMD_EXIT:
			putline_console(cmd, "exit program");
			exit(0);
		case CMD_UNKNOWN:
			putline_console(cmd, "error : unknown command");
			break;
		default:
			putline_console(cmd, "error : cannot retrieve command");
	}

	putline_console(cmd, "\n> ");
	cmd->limx = cmd->posx;
	cmd->limy = cmd->posy;
	cmd->cmd_id = 0;

	show_hint(cmd);
}

void OnLoop()
{
	float ElapsedTime = 0.f;
	SDL_Event event;

	SDL_EnableKeyRepeat(150, 30);
	SDL_EnableUNICODE(1);
	start(t);
	while(1)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					exit(0);
				case SDL_KEYDOWN:
					key_state[event.key.keysym.sym] = true;
					if(cmd->show)
					{
						input_console(cmd, &event);
					}

					switch(event.key.keysym.sym)
					{
						case SDLK_F1:
							toggleConsole(cmd);
							break;
						case SDLK_F2:
							glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
							break;
						case SDLK_F3:
							glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
							break;
					}
					break;
				case SDL_KEYUP:
					key_state[event.key.keysym.sym] = false;
					break;
				case SDL_MOUSEMOTION:
					if(!cmd->show)
					{
						setCamera(cam,  event.motion.xrel, event.motion.yrel);
					}
					break;
			} 

		}

		if(key_state[SDLK_ESCAPE])
		{
			exit(0);
		}

		ElapsedTime = (float)lap(t);
		if(ElapsedTime >= (1000.f/FPS))
		{
			// changement input
			if(!cmd->show)
			{
				setCamera(cam, key_state, ElapsedTime);
			}
			else
				OnProcessCommand();
			
			start(t);
			// Affichage
			OnRender();

		}
	}
}

int main(int argc, char** argv){
	

	/* initialisation de la fenêtre */
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("Terrain Generator", NULL);
	SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);

	// Mettre en place la vue
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, 800.0/600.0, 1.0, 1000000.0);

	glewInit();
	// Initialisation des GObjets
	OnCreate();
	
	/* Initialisation d'OpenGL */
	glClearColor(0.f,0.f,0.1f,0.f);
	
	/* On passe à une taille de 1 pixel */
	glPointSize(1.0);
	
	/* entrée dans la boucle principale de glut */
	OnLoop();

	return EXIT_SUCCESS;
}

void OnRelease(){
	releaseAll();

	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(SDL_ENABLE);
	SDL_Quit();
}
