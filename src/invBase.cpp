#include "invMath.h"
#include "invGeom.h"
#include "invTex.h"
#include "invTalk.h"
#include "invBase.h"

//------------------------------------------------
// Game
//------------------------------------------------
int Game::fps;
bool Game::texture = true;
Entity* Game::pSelect = NULL;
void Game::bind(BIND_TYPE type, void* func)
{
	switch(type)
	{
		case STOP_WHEN_FUNCTION:
			stopwhen = (event_function)func;	
			break;
		case INPUT_FUNCTION:
			input = (input_function)func;
			break;
		case MOTION_FUNCTION:
			motion = (tick_function)func;
			break;
	}
}

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
			if(Input::keychar == SDLK_F1)
				Interface::cons->show = !Interface::cons->show;
			if(Interface::cons->show)
				(*inputInter)();

			float ElapsedTime = (float)(now-before);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			if(!Interface::cons->show)
				(*inputCamera)(ElapsedTime);
			(*lookCamera)();
			glPushMatrix();

			static GLfloat lightcolor[] = {0.8f, 0.5f, 0.f};
			static GLfloat lightpos[] = {10.f, 10.f, -1.f};
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
			while(pCopy)
			{
				// déplacer l'entité TODO améliorer le processus de déplacement
				if(pCopy->type & Entity::mobile)
				{
					float deltaX = pCopy->dest.x - pCopy->trans.x;
					float deltaZ = -(pCopy->dest.z - pCopy->trans.z);
					float length = sqrtf(deltaX * deltaX + deltaZ * deltaZ);
					if(pCopy->state == Entity::walking && !deltaX && !deltaZ)
					{
						(*pCopy->set_animation)(pCopy->mesh, 0);
						pCopy->state = Entity::stand;
					}

					if(pCopy->state == Entity::walking)
					{
						if(!pCopy->oriented)
						{
							float angle = acosf(deltaX / length) * 180/F_PI;
							if(deltaZ < 0.f)
								angle=-angle;
							pCopy->rot.eulerAngle(pCopy->angleX, -angle, pCopy->angleZ);
							pCopy->oriented = true;
						}

						else
						{
							float forwardX = deltaX/length * 0.05f;	
							float forwardZ = deltaZ/length * 0.05f;	
							if(length >= 0.05f)
							{
								pCopy->trans.x += forwardX;
								pCopy->trans.z -= forwardZ;
							}
							else
							{
								pCopy->trans.x = pCopy->dest.x;
								pCopy->trans.z = pCopy->dest.z;
							}
						}
					}
				}

			
				pCopy->rot.getMatrix(m);

				glPushMatrix();
				glTranslatef(pCopy->trans.x, pCopy->trans.y, pCopy->trans.z);
				glMultMatrixf(m);
				glScalef(pCopy->scale.x, pCopy->scale.y, pCopy->scale.z);
				if(pCopy->type & Entity::transparent_textures)
				{
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				}

				(*pCopy->render_function)(pCopy->mesh, ElapsedTime);

				if(pCopy->type & Entity::transparent_textures)
					glDisable(GL_BLEND);
				glPopMatrix();

				pCopy = pCopy->pNext;
			}

			//---------- 2D Rendering -------------- 
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0.0, (GLdouble)Game::width, (GLdouble)Game::height, 0.0, -10.0,  10.0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			(*renderInter)(fontInter);
			SDL_GL_SwapBuffers();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			//---------- Picking -------------- 
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Game::texture = false;
			pCopy = pEntityList;
			while(pCopy)
			{
				glPushMatrix();
				glColor3ub(pCopy->id, 0, 0);
				glTranslatef(pCopy->trans.x, pCopy->trans.y, pCopy->trans.z);
				glMultMatrixf(m);
				glScalef(pCopy->scale.x, pCopy->scale.y, pCopy->scale.z);
				(*pCopy->render_function)(pCopy->mesh, ElapsedTime);
				glPopMatrix();

				pCopy = pCopy->pNext;
			}

			Uint8 red;
			glReadPixels(Input::mousex, Game::height - Input::mousey, 1, 1, GL_RED, GL_UNSIGNED_BYTE, (void*)&red);

			pCopy = pEntityList;
			pSelect = NULL;
			while(pCopy){
				if(pCopy->id == red){
					pSelect = pCopy;
					break;
				}
				pCopy = pCopy->pNext;
			}

			Game::texture = true;
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

Entity* Game::addMesh(void* mesh, void (*render_function)(void*, float), void (*set_animation)(void*, int), int type, Vec3f trans, Vec3f scale, float angleX, float angleY, float angleZ)
{
	Entity* e = new Entity;
	e->mesh = mesh;
	e->render_function = render_function;
	e->set_animation = set_animation;
	e->pProList = NULL;

	e->dest = trans;
	e->trans = trans;
	e->scale = scale;
	e->rot.eulerAngle(angleX, angleY, angleZ);
	e->angleX = angleX; e->angleY = angleY; e->angleZ = angleZ;
	e->id = id_head++;
	e->selected = false;
	e->oriented = true;
	e->state = Entity::stand;
	e->type = type;

	e->pNext = pEntityList;
	pEntityList = e;

	return e;
}

void Game::goTo(Entity* e, float x, float y, float z)
{
	e->dest.x = x; e->dest.y = y; e->dest.z = z;
	e->state = Entity::walking;
	e->oriented = false;
	(*e->set_animation)(e->mesh, 1);
}

Entity* Game::getSelected()
{
	Entity* pCopy = pEntityList;
	while(pCopy)
	{
		if(pCopy->selected)	
			return pCopy;
		pCopy = pCopy->pNext;
	}
}

//------------------------------------------------
// GarbageCollector
//------------------------------------------------
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

//------------------------------------------------
// Scenery
//------------------------------------------------
void Scenery::drawCube(float x, float y, float z)
{
	glBegin(GL_QUADS);
	
    glVertex3f(  x,  y, -z );
    glVertex3f( -x,  y, -z );
    glVertex3f( -x,  y,  z );
    glVertex3f(  x,  y,  z );

    glVertex3f(  x, -y,  z );
    glVertex3f( -x, -y,  z );
    glVertex3f( -x, -y, -z );
    glVertex3f(  x, -y, -z );

    glVertex3f(  x,  y,  z );
    glVertex3f( -x,  y,  z );
    glVertex3f( -x, -y,  z );
    glVertex3f(  x, -y,  z );
  
    glVertex3f(  x, -y, -z );
    glVertex3f( -x, -y, -z );
    glVertex3f( -x,  y, -z );
    glVertex3f(  x,  y, -z );

    glVertex3f( -x,  y,  z );
    glVertex3f( -x,  y, -z );
    glVertex3f( -x, -y, -z );
    glVertex3f( -x, -y,  z );
  
    glVertex3f(  x,  y, -z );
    glVertex3f(  x,  y,  z );
    glVertex3f(  x, -y,  z );
    glVertex3f(  x, -y, -z );
    glEnd();
}
