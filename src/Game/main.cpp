#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL/SDL_image.h>
#define GLX_GLXEXT_PROTOTYPES 1
#include <GL/glx.h>
#include "../invLib/invMath.h"
#include "../invLib/invGeom.h"
#include "../invLib/invTex.h"
#include "../invLib/invTalk.h"
#include "../invLib/invBase.h"

Text::INV_Font* font;
GLuint terrainTex[1];
IMF::Model* stickman, *maison;
Vec3f pos_tree;

void move(float ElapsedTime)
{
	if(Input::mouseButton & SDL_BUTTON(3))
	{
		Entity* select = Game::pESelect;
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
			Game::goTo(Game::pISelect, (float)objx, 0.f, (float)objz);
			static char buffer[32];
			sprintf(buffer, "entity going to %f %f\0", objx, objz);
			Interface::print(buffer);
		}
	}
}

int main(int argc, const char *argv[])
{
	if(!Game::init(1024, 768, false)){
		return EXIT_FAILURE;
	}
	
	GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	glEnable(GL_LIGHT0);
	glEnable(GL_SMOOTH);

	srand(time(NULL));

	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_PumpEvents();
	SDL_WarpMouse((Uint16)Game::width/2, (Uint16)Game::height/2);
	if(!Text::init()){
		return EXIT_FAILURE;
	}
	font = Text::genTextures(Text::loadFont("/usr/share/fonts/truetype/freefont/FreeMono.ttf", 14));

	Game::setTitle("Invasion v0.0.3");
	Game::bind(Game::INPUT_FUNCTION, (void*)Input::getInput);
	Game::bind(Game::STOP_WHEN_FUNCTION, (void*)Game::isPressed);
	Game::bind(Game::MOTION_FUNCTION, (void*)move);
	Game::bindInterface(Interface::init, Interface::render, Interface::input, font);
	Game::bindCamera(Camera::lookRTS, Camera::mouseRTS);
	Register::add("exit", "", Game::exit, NULL);
	Register::add("camera", "s", Camera::switchCameraView, NULL);

	void (*swapInterval) (int);
	swapInterval = (void (*)(int))glXGetProcAddress((const GLubyte*)"glXSwapIntervalSGI");
	// (*swapInterval)(1);

	Terrain::tex = Tex::convertFromSDLSurface(IMG_Load("../../Textures/Path.png"));

	Terrain::init();
	Terrain::data = Array2D::plane(3);
	Terrain::width = Terrain::length = 3;
	Terrain::size = 10.f;
	Terrain::tex_size = 10.f;

	stickman = IMF::load("../../3DModels/Objs/stickman.imf");
	maison = IMF::load("../../3DModels/Objs/maison_age1.imf");
	Entity* stickman_e = Game::addMesh((void*)stickman, IMF::render, NULL, Entity::mobile|Entity::complete_transparent, Vec3f(0.f, 0.25f, 0.f), Vec3f(0.3f, 0.3f, 0.3f), Vec3f(0.f, 0.f, 0.f));
	Entity* maison_e = Game::addMesh((void*)maison, IMF::render, NULL, Entity::transparent_textures, Vec3f(0.f, 0.0001f, 0.f), Vec3f(0.5f, 0.5f, 0.5f), Vec3f(0.f, 0.f, 0.f));
	Game::createInstance(maison_e, Vec3f(-1.f, 0.f, -2.f), Vec3f(135.f, 0.f, 0.f));
	Game::createInstance(maison_e, Vec3f(1.f, 0.f, 3.f), Vec3f(135.f, 0.f, 0.f));
	Game::createInstance(maison_e, Vec3f(3.f, 0.f, 0.f), Vec3f(250.f, 0.f, 0.f));
	Game::createInstance(stickman_e, Vec3f(0.f, 0.f, 0.f), Vec3f(0.f, 0.f, 0.f));
	maison->tex = Tex::convertFromSDLSurface(IMG_Load("../../Textures/maison_age1.png"));
	stickman->tex = Tex::convertFromSDLSurface(IMG_Load("../../Textures/stickman.png"));
	Game::mainLoop(); 
	glDeleteTextures(1, &Terrain::tex);
	GarbageCollector::release_all();
	return 0;
}
