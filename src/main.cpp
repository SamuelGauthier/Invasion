#define FPS 60
#define WIDTH_WINDOW 800
#define HEIGHT_WINDOW 600
#define TEST 0

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <unistd.h>
#include "utils.h"
#include "camera.h"
#include "mesh.h"
#include "text.h"
#include "terrain.h"
#include "ui.h"
#include "console.h"

// Variables globales
GCamera* cam = 0;
GOBJModel* traffic_cone = 0;
GConsole* cmd = 0;
GTerrain* trn = 0;

GFont* f = 0;

bool key_state[256] = {false};

// Prototypes de fonctions
inline void OnCreate();
void OnRelease();
inline void OnRender();
inline void OnRender2D();
inline void OnLoop();

void OnCreate(){
	cam = initCamera(freefly_camera);

	// init Font
	f = loadFont("../Font/Test.fnt");

	// init console
	cmd = initConsole();

	// load models
	traffic_cone = loadOBJ("../3DModels/traffic_cone_001.obj");
	setTexture(traffic_cone, "../Textures/traffic_cone_tex.tga");

	// textures
	trn = initTerrain();

	planeTerrain(trn, 5);
	setTexture(trn, "../Textures/grass.tga",0);
	trn->cell = 10.f;
	fillBuffers(trn); 
	
	// init state
	glEnable(GL_DEPTH_TEST);

	// exit
	atexit(OnRelease);
}

void OnRender(){

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
	lookCamera(cam);

	glPushMatrix();
	glScalef(3.f,3.f,3.f);
	glRotatef(-90.f,1.f,0.f,0.f);
	render(traffic_cone);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	render(trn);
	glDisable(GL_TEXTURE_2D);

	/*
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

	*/
	//---------------------------------
	// End Render 2D
	//---------------------------------

	SDL_GL_SwapBuffers();
}

void OnRender2D()
{
	render(cmd, f);
}

void OnLoop()
{
	uint fps = 1000/FPS, begin = 0;
	SDL_Event event;

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
					break;
				case SDL_KEYUP:
					key_state[event.key.keysym.sym] = false;
					break;
				case SDL_MOUSEMOTION:
					setCamera(cam,  event.motion.xrel, event.motion.yrel);
					break;
			}

		}
		if(key_state[SDLK_ESCAPE])
		{
			exit(0);
		}

		
		if(SDL_GetTicks() - fps >= begin)
		{
			// changement input
			setCamera(cam, key_state);


			// Affichage
			OnRender();

			begin = SDL_GetTicks();
		}
	}
}

int main(int argc, char** argv){
	/* initialisation de la fenêtre */
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("OpenGL test", NULL);
	SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);

	// Mettre en place la vue
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 800.0/600.0, 1.0, 200.0);

	// Initialisation des GObjets
	OnCreate();
	
	/* Initialisation d'OpenGL */
	glClearColor(0.f,0.f,0.1f,0.f);
	
	/* On passe à une taille de 1 pixel */
	glPointSize(1.0);
	
	/* entrée dans la boucle principale de glut */
	OnLoop();

}

void OnRelease(){
	releaseAll();
	SDL_Quit();
}
