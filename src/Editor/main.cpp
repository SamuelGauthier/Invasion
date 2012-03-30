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

IMF::Model* maison = NULL;
Text::INV_Font* font;
GUI::Control* ppanel, *opanel, *propanel, *coutpanel;
GUI::Control* ebname;
GUI::Control* listmodel;

void move(float ElapsedTime)
{
	
}

void exit_event()
{
	Game::exit(NULL, NULL);
}

void new_event()
{
	GUI::parentTo(ppanel, propanel);
}

void save_event()
{
	bool found = false;
	ModelManager::ModelProp* copy = ModelManager::pModelList;
	while(copy)
	{
		if(!strcmp(copy->name, ebname->text))
		{
			found = true;
			break;
		}
		copy = copy->pNext;
	}
	
	if(!found)
	{
		ModelManager::ModelProp* mp = new ModelManager::ModelProp;
		strncpy(mp->name, ebname->text, 16);
		mp->name[15] = '\0';
		mp->pNext = ModelManager::pModelList;
		ModelManager::pModelList = mp;

		delete[] listmodel->text;
		listmodel->text = ModelManager::getList();
	}
}

void select_event()
{
	new_event();

	if(*listmodel->text)
	{
		char* newline;
		ModelManager::ModelProp* mp = ModelManager::search(GUI::selectedList(listmodel, &newline));
		*newline = '\n';
		delete[] ebname->text;
		ebname->text = new char[strlen(mp->name)+1];
		strcpy(ebname->text, mp->name);
		ebname->text[strlen(mp->name)] = '\0';
	}
}

void delete_event()
{
	if(*listmodel->text)
	{
		char* dummy;
		ModelManager::remove(GUI::selectedList(listmodel, &dummy));
		delete[] listmodel->text;
		listmodel->text = ModelManager::getList();
	}
}

int main(int argc, const char *argv[])
{
	if(!Game::init(1024, 768, false)){
		return EXIT_FAILURE;
	}
	
	GLfloat ambientColor[] = {0.1f, 0.1f, 0.1f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	glEnable(GL_LIGHT0);
	glEnable(GL_SMOOTH);
	srand(time(NULL));

	SDL_PumpEvents();
	SDL_WarpMouse((Uint16)Game::width/2, (Uint16)Game::height/2);
	if(!Text::init()){
		return EXIT_FAILURE;
	}
	font = Text::genTextures(Text::loadFont("/usr/share/fonts/truetype/freefont/FreeMono.ttf", 14));

	ModelManager::load();
	GUI::init();
	ppanel = GUI::addPanel(GUI::window, Game::width - 300, 0, 300, Game::height);
	opanel = GUI::addPanel(ppanel, 20, 20, 260, 65);
	propanel = GUI::addPanel(ppanel, 20, 100, 260, 500);
	coutpanel = GUI::addPanel(propanel, 20, 70, 220, 110);
	GUI::addButton(ppanel, "Quit", 200, 730, exit_event);
	char* pmodels = ModelManager::getList();
	listmodel = GUI::addList(opanel, pmodels, 10, 10, 240);
	GUI::addButton(opanel, "New", 10, 35, new_event, 60);
	GUI::addButton(opanel, "Select", 80, 35, select_event, 70);
	GUI::addButton(opanel, "Delete", 160, 35, delete_event, 90);
	GUI::addLabel(propanel, "Name", 20, 20);
	ebname = GUI::addTextBox(propanel, "unknown", 100, 20, 140);
	GUI::addLabel(propanel, "Class", 20, 45);
	char choice[] = "Villager\nHouse\nWarrior\nAnimal\nScenery\n";
	GUI::addList(propanel, choice, 100, 45, 140);
	GUI::addLabel(coutpanel, "Wood", 10, 20);
	GUI::addTextBox(coutpanel, "100", 80, 20, 120);
	GUI::addLabel(coutpanel, "Gold", 10, 45);
	GUI::addTextBox(coutpanel, "100", 80, 45, 120);
	GUI::addLabel(coutpanel, "Stone", 10, 70);
	GUI::addTextBox(coutpanel, "100", 80, 70, 120);
	GUI::addButton(propanel, "Save", 170, 470, save_event);
	GUI::unparentOf(ppanel, propanel);

	Game::setTitle("InvEditor v0.0.5");
	Game::bind(Game::INPUT_FUNCTION, (void*)Input::getInput);
	Game::bind(Game::STOP_WHEN_FUNCTION, (void*)Game::isPressed);
	Game::bind(Game::MOTION_FUNCTION, (void*)move);
	Game::bindInterface(Interface::init, Interface::render, Interface::input, font);
	Game::bindCamera(Camera::lookFixed, Camera::mouseFixed);
	Register::add("exit", "", Game::exit, NULL);

	void (*swapInterval) (int);
	swapInterval = (void (*)(int))glXGetProcAddress((const GLubyte*)"glXSwapIntervalSGI");
	// (*swapInterval)(1);

	Terrain::init();
	Terrain::width = Terrain::length = 0;
	
	maison = IMF::load("../../3DModels/Objs/maison_age1.imf");
	maison->tex = Tex::convertFromSDLSurface(IMG_Load("../../Textures/maison_age1.png"));
	Entity* maison_e = Game::addMesh((void*)maison, IMF::render, NULL, 0, Vec3f(0.f, 0.f, 0.f), Vec3f(1.f, 1.f, 1.f), Vec3f(0.f, 0.f, 0.f));
	Game::createInstance(maison_e, Vec3f(0.f, 0.f, 0.f), Vec3f(0.f, 0.f, 0.f));
	Game::mainLoop(); 
	GarbageCollector::release_all();
	return 0;
}
