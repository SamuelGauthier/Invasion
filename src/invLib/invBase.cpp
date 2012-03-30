#include "invMath.h"
#include "invGeom.h"
#include "invTex.h"
#include "invTalk.h"
#include "invBase.h"

//------------------------------------------------
// Game
//------------------------------------------------
int Game::width = 0, Game::height = 0;
int Game::fps;
bool Game::texture = true;
Entity* Game::pESelect = NULL;
Instance* Game::pISelect = NULL;
Entity* Game::pEntityList = NULL;

void Game::mainLoop()
{
	SDL_Event event;
	Uint32 before = 0, now, time_delay = 1000/fps;

	SDL_EnableKeyRepeat(100, 40);
	SDL_EnableUNICODE(1);
	Entity* pCopy;
	while(!Input::quit && !(*stopwhen)())
	{
		
		now = SDL_GetTicks();
		if(time_delay < (now - before))
		{
			
			(*input)();
			// GUI::focused(GUI::window);
			// if(Input::keychar == SDLK_F1)
				// Interface::cons->show = !Interface::cons->show;
			// if(Interface::cons->show)
				// (*inputInter)();

			float ElapsedTime = (float)(now-before);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			if(!Interface::cons->show )// && !GUI::focusedGUI)
				(*inputCamera)(ElapsedTime);
			(*lookCamera)();
			glPushMatrix();

			static GLfloat lightcolor[] = {1.0f,1.0f,1.0f};
			static GLfloat lightpos[] = {0.f, 4.f, 10.f};
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcolor);
			glLightfv(GL_LIGHT0, GL_POSITION, lightpos);


			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, Terrain::tex);
			Terrain::render();
			glDisable(GL_TEXTURE_2D);

			//---------- Motion --------------------
			(*motion)(ElapsedTime);

			//---------- 3D Rendering -------------- 
			pCopy = pEntityList;
			static float m[16];
			static Instance* pProList;
			while(pCopy)
			{
				// déplacer l'entité TODO améliorer le processus de déplacement
				if(pCopy->type & Entity::mobile)
				{
					pProList = pCopy->pProList;

					while(pProList)
					{
						float deltaX = pProList->dest.x - pProList->pos.x;
						float deltaZ = -(pProList->dest.z - pProList->pos.z);
						float length = sqrtf(deltaX * deltaX + deltaZ * deltaZ);
						if(pProList->state == Entity::walking && !deltaX && !deltaZ)
						{
							// (*pCopy->set_animation)(pCopy->mesh, 0);
							pProList->state = Entity::stand;
						}

						if(pProList->state == Entity::walking)
						{
							if(!pProList->oriented)
							{
								float angle = acosf(deltaX / length) * 180/F_PI;
								if(deltaZ < 0.f)
									angle=-angle;
							  	pProList->dir.x = -angle;
								pProList->oriented = true;
								pProList->rot.eulerAngle(pProList->dir.x, pProList->dir.y, pProList->dir.z);
							}

							else
							{
								float forwardX = deltaX/length * 0.05f;	
								float forwardZ = deltaZ/length * 0.05f;	
								if(length >= 0.05f)
								{
									pProList->pos.x += forwardX;
									pProList->pos.z -= forwardZ;
								}
								else
								{
									pProList->pos.x = pProList->dest.x;
									pProList->pos.z = pProList->dest.z;
								}
							}
						}

						pProList = pProList->pNext;
					}
				}

				if(pCopy->type & Entity::transparent_textures)
				{
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				}

				if(pCopy->type & Entity::complete_transparent)
				{
					glEnable(GL_ALPHA_TEST);
					glAlphaFunc(GL_GREATER, 0.5);
				}


				pProList = pCopy->pProList;
				while(pProList)
				{
					glPushMatrix();
					glTranslatef(pProList->pos.x, pProList->pos.y, pProList->pos.z);
					pProList->rot.getMatrix(m);
					pProList->dir.x += 0.1f;
					pProList->rot.eulerAngle(pProList->dir.x, pProList->dir.y, pProList->dir.z);
					glMultMatrixf(m);
					glScalef(pCopy->scale.x, pCopy->scale.y, pCopy->scale.z);
					(*pCopy->render_function)(pCopy->mesh, ElapsedTime);
					glPopMatrix();
					pProList = pProList->pNext;
				}

				if(pCopy->type & Entity::complete_transparent)
				{
					glDisable(GL_ALPHA_TEST);
				}

				if(pCopy->type & Entity::transparent_textures)
					glDisable(GL_BLEND);

				pCopy = pCopy->pNext;
			}

			//---------- 2D Rendering -------------- 
			// GUI::interact(GUI::window);

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0.0, (GLdouble)Game::width, (GLdouble)Game::height, 0.0, -100.0,  100.0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			(*renderInter)(fontInter);
			// GUI::render(GUI::window, fontInter, -8);
			SDL_GL_SwapBuffers();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			//---------- Picking -------------- 
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			if(Input::mouseButton & SDL_BUTTON(1))
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				Game::texture = false;
				pCopy = pEntityList;
				while(pCopy)
				{
					pProList = pCopy->pProList;
					while(pProList)
					{
						glPushMatrix();
						glColor3ub(pProList->id, 0, 0);
						glTranslatef(pProList->pos.x, pProList->pos.y, pProList->pos.z);
						pProList->rot.getMatrix(m);
						glMultMatrixf(m);
						glScalef(pCopy->scale.x, pCopy->scale.y, pCopy->scale.z);
						(*pCopy->render_function)(pCopy->mesh, ElapsedTime);
						glPopMatrix();

						pProList = pProList->pNext;
					}
					pCopy = pCopy->pNext;
				}

				Uint8 red;
				glReadPixels(Input::mousex, Game::height - Input::mousey, 1, 1, GL_RED, GL_UNSIGNED_BYTE, (void*)&red);

				pESelect = NULL;
				pISelect = NULL;
				pCopy = pEntityList;
				while(pCopy){
					pProList = pCopy->pProList;
					while(pProList)
					{
						if(pProList->id == red){
							pESelect = pCopy;
							pISelect = pProList;
							break;
						}
						pProList = pProList->pNext;
					}
					pCopy = pCopy->pNext;
				}
				Game::texture = true;
			}
			before = now;
		}
		
		else
		{
			SDL_Delay(time_delay - (now - before));
		}
	}
}

bool Game::isPressed()
{
	return Input::key[SDLK_ESCAPE];
}

void Game::exit(char**, void*)
{
	Input::quit = true;
}

void Game::bindInterface(init_function it, write_function wt, input_function ip, Text::INV_Font* font)
{
	initInter = it;
	renderInter = wt;
	inputInter = ip;

	(*initInter)();

	fontInter = font;
}

void Game::bindCamera(void_function lookCam, tick_function inputCam)
{
	lookCamera = lookCam;
	inputCamera = inputCam;
}

Entity* Game::addMesh(void* mesh, void (*render_function)(void*, float), void (*set_animation)(void*, int), int type, Vec3f trans, Vec3f scale, Vec3f dir)
{
	Entity* e = new Entity;
	e->mesh = mesh;
	e->render_function = render_function;
	e->set_animation = set_animation;
	e->pProList = NULL;

	e->trans = trans;
	e->scale = scale;
	e->rot.eulerAngle(dir.x, dir.y, dir.z);
	e->dir = dir;
	e->type = type;

	e->pNext = pEntityList;
	pEntityList = e;

	return e;
}

void Game::createInstance(Entity* e, Vec3f pos, Vec3f dir)
{
	Instance* i = new Instance;
	i->pos = pos + e->trans;
	i->dir = dir + e->dir;
	i->rot.eulerAngle(i->dir.x, i->dir.y, i->dir.z);
	i->id = id_head++;

	i->pNext = e->pProList;
	e->pProList = i;
}

void Game::goTo(Instance* p, float x, float y, float z)
{
	p->dest.x = x; p->dest.y = y; p->dest.z = z;
	p->state = Entity::walking;
	p->oriented = false;
	// (*e->set_animation)(e->mesh, 1);
}

void Game::new_entity(char* arg[], void*)
{
	IMF::Model* m = IMF::load(arg[0]);
	if(!m)
	{
		char buffer[64];
		strcpy(buffer, "../3DModels/Objs/");
		strcat(buffer, arg[0]);

		m = IMF::load(buffer);
		if(!m)
			return;
	}

	SDL_Surface* img = IMG_Load(arg[1]);
	if(!img)
	{
		char buffer[64];
		strcpy(buffer, "../Textures/");
		strcat(buffer, arg[1]);

		img = IMG_Load(arg[1]);
		if(!img)
			return;
	}
	m->tex = Tex::convertFromSDLSurface(img);
	Entity* e = addMesh((void*)m, IMF::render, NULL, 0, Vec3f(0.f, 0.0001f, 0.f), Vec3f(1.f, 1.f, 1.f), Vec3f(0.f, 0.f, 0.f));
	Game::createInstance(e, Vec3f(0.f, 0.f, 0.f), Vec3f(0.f, 0.f, 0.f));
}

//------------------------------------------------
// Model Manager
//------------------------------------------------
ModelManager::ModelProp* ModelManager::pModelList = NULL;
void ModelManager::load()
{
	GarbageCollector::add(unload, NULL);
	FILE* data = fopen("index.mlb" ,"rb");
	if(!data)
		return;

	fseek(data, 0, SEEK_END);
	int count = ftell(data)/128;
	rewind(data);
	
	ModelProp* mp = NULL;
	for(int i=0;i<count;i++)
	{
		mp = read(data);
		if(!mp)
			break;
		mp->pNext = pModelList;
		pModelList = mp;
	}

	fclose(data);
}

ModelManager::ModelProp* ModelManager::search(char* name)
{
	ModelProp* mp = pModelList;
	while(mp)
	{
		if(!strcmp(mp->name, name))
			break;
		mp = mp->pNext;
	}
	return mp;
}

void ModelManager::remove(char* name)
{
	ModelProp* previous = NULL;
	ModelProp* mp =	pModelList; 
	while(mp)
	{
		if(!strcmp(mp->name, name))
		{
			if(!previous)
			{
				pModelList = mp->pNext;
				delete mp;
			}
			else
			{
				previous->pNext = mp->pNext;
				delete mp;
			}
			break;
		}
		previous = mp;
		mp = mp->pNext;
	}
}

void ModelManager::unload(void*)
{
	FILE* data = fopen("index.mlb", "wb");
	while(pModelList)
	{
		ModelProp* tmp = pModelList;
		pModelList = pModelList->pNext;
		write(data, tmp);
		delete tmp;
	}
	fclose(data);
}

ModelManager::ModelProp* ModelManager::read(FILE* base)
{
	if(feof(base))
		return NULL;
	ModelProp* mp = new ModelProp;
	char buffer[128];
	fread((void*)buffer, 1, 128, base);
	strncpy(mp->name, &buffer[0], 16);
	return mp;
}

void ModelManager::write(FILE* base, ModelProp* mp)
{
	char buffer[128];
	memset((void*)buffer, '\0', 128);
	fwrite((void*)mp->name, 1, 16, base);
	fwrite((void*)buffer, 1, 128-16, base); // reserved
}

char* ModelManager::getList()
{
	char* buffer = new char[128];
	char* str = buffer;
	*str = '\0';

	ModelProp* copy = pModelList;
	while(copy)
	{
		strcat(str, copy->name);
		str[strlen(copy->name)] = '\n'; // replace '\0' by '\n'
		str += (strlen(copy->name) + 1);
		*str = '\0';
		copy = copy->pNext;
	}

	return buffer;
}
//------------------------------------------------
// GarbageCollector
//------------------------------------------------
GarbageCollector::GCEntry* GarbageCollector::pHead = NULL;
void GarbageCollector::init()
{
	pHead = NULL;
}

void GarbageCollector::add(release_function func, void* arg)
{
	GCEntry* e = new GCEntry;
	e->func = func;
	e->arg  = arg;

	e->pNext = pHead;
	pHead = e;
}

void GarbageCollector::release_all()
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
