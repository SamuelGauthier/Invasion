#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL/SDL_image.h>
#define GLX_GLXEXT_PROTOTYPES 1
#include <GL/glx.h>
#include "invMath.h"
#include "invGeom.h"
#include "invTex.h"
#include "invTalk.h"
#include "invBase.h"

int Game::width = 0, Game::height = 0;
Text::INV_Font* font;
GLuint terrainTex[1];
IMF::Model* house;
MD2::Model* drfreak;
Vec3f pos_tree;

void move(float ElapsedTime)
{
	if(Game::pSelect && Input::mouseButton & SDL_BUTTON(1))
	{
		Game::pSelect->selected = true;
		printf("grand mother selected\n");
	}

	if(Input::mouseButton & SDL_BUTTON(3))
	{
		Entity* select = Game::getSelected();
		if(select && select->type & Entity::mobile)
		{
			GLdouble objx, objy, objz;
			GLint viewport[4];
			GLdouble modelview[16], projection[16];
			GLfloat winX, winY, winZ;
			glGetIntegerv(GL_VIEWPORT, viewport);
			glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
			glGetDoublev(GL_PROJECTION_MATRIX, projection);

			winX = (GLfloat)Input::mousex;
			winY = (GLfloat)(Game::height - Input::mousey);
			glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

			gluUnProject(	winX, winY, winZ,
							modelview, projection, viewport,
							&objx, &objy, &objz);

			Game::goTo(select, (float)objx, 0.f, (float)objz);
		}
	}
}

int main(int argc, const char *argv[])
{
	if(!Game::init(1024, 768, false)){
		return EXIT_FAILURE;
	}
	
	GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	glEnable(GL_LIGHT0);
	glEnable(GL_SMOOTH);
	glEnable(GL_LIGHTING);

	srand(time(NULL));

	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_PumpEvents();
	SDL_WarpMouse((Uint16)Game::width/2, (Uint16)Game::height/2);
	if(!Text::init()){
		return EXIT_FAILURE;
	}
	font = Text::genTextures(Text::loadFont("/usr/share/fonts/truetype/freefont/FreeMono.ttf", 16));

	Game::setTitle("InvEditor v0.0.3");
	Game::bind(Game::INPUT_FUNCTION, (void*)Input::getInput);
	Game::bind(Game::STOP_WHEN_FUNCTION, (void*)Game::isPressed);
	Game::bind(Game::MOTION_FUNCTION, (void*)move);
	Game::bindInterface(Interface::init, Interface::render, Interface::input, font);
	Game::bindCamera(Camera::lookFPS, Camera::mouseFPS);
	Register::add("exit", "", Game::exit, NULL);
	Register::add("camera", "s", Camera::switchCameraView, NULL);

	void (*swapInterval) (int);
	swapInterval = (void (*)(int))glXGetProcAddress((const GLubyte*)"glXSwapIntervalSGI");
	// (*swapInterval)(1);

	Terrain::tex = Tex::convertFromSDLSurface(IMG_Load("paves.jpg"));

	Terrain::init();
	Terrain::data = Array2D::plane(2);
	Terrain::width = Terrain::length = 2;
	Terrain::size = 10.f;

	house = IMF::load("../3DModels/Objs/maison_age1.imf");
	drfreak = MD2::load("drfreak.md2");
	drfreak->tex = Tex::convertFromSDLSurface(IMG_Load("drfreak.tga"));
	Game::addMesh((void*)house, IMF::render, NULL, Entity::transparent_textures, Vec3f(0.f, 0.1f, 0.f), Vec3f(1.f, 1.f, 1.f), 0.f, 0.f, 0.f);
	house->tex = Tex::convertFromSDLSurface(IMG_Load("../Textures/maison_age1.png"));
	Game::mainLoop(); 
	glDeleteTextures(1, &Terrain::tex);
	GarbageCollector::release_all();
	return 0;
}
