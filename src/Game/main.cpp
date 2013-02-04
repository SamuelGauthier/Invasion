#include <cstdio>
#include <cstdlib>
#include <Windows.h>
#include <GL/glew.h>

#include "invRTS.h"
#include "invBase.h"
#include "invScene.h"
#include "invTex.h"
#include "invGeom.h"
#include "invTalk.h"
#include "invManager.h"
#include "invGrid.h"
#include "invInput.h"


#define COMPENSATION 16

invContext* cxt;
invFlow flow;
invCamera cam(RTS_CAM);
invRTSGame game;
// invShader sh;

float elapsedTime = 0.f;
float before = 0;
int frames = 0;
GLuint circle;
invEntity* turret = NULL;
float cam_speed = 0.f;
float cam_angle = -50.f;

float posx = 100.f;
float lightPos[4] = {2.5f, 47.5f, 56.f, 1.f};
float globalAmbient = 4.45f;
float shininess = 59.5f;

bool showPicking = false;

inline void print_infos()
{
}

void initLight()
{
	GLfloat LightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);

	GLfloat LightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);

	// GLfloat LightSpecular[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    // glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);

	glEnable(GL_LIGHT0);

	// glUniform3f(glGetUniformLocation(sh.program, "colorShading"), 1.f, 0.9f, 0.6f);
}


void checkEvent()
{
	updateInputData();
	if(sinput->keychar == SDLK_ESCAPE)
		flow.stop();
	if(sinput->keychar == SDLK_F5)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if(sinput->keychar == SDLK_F6)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if(sinput->keys[SDLK_q])
	{
		if(cam_speed < 1.f)
			cam_speed += 0.005f;
	}

	else if(sinput->keys[SDLK_e])
	{
		if(cam_speed > -1.f)
			cam_speed -= 0.005f;
	}

	else
	{
		cam_speed += -cam_speed/20.f;
	}

	if(sinput->keys[SDLK_y])
		cam_angle += 1.f;
	if(sinput->keys[SDLK_x])
		cam_angle -= 1.f;
	if(sinput->keychar == SDLK_F8)
		showPicking = !showPicking;

	cam.setPos(cam.pos + Vec3f(0.f, 0.5f, 0.5f) * cam_speed);
	cam.setAngles(0.f, cam_angle);

	if(sinput->keys[SDLK_w]) { lightPos[1] += 0.1f; }
	if(sinput->keys[SDLK_s]) { lightPos[1] -= 0.1f; }
	if(sinput->keys[SDLK_a]) { lightPos[0] += 0.1f; }
	if(sinput->keys[SDLK_d]) { lightPos[0] -= 0.1f; }
	if(sinput->keys[SDLK_r]) { lightPos[2] += 0.1f; }
	if(sinput->keys[SDLK_f]) { lightPos[2] -= 0.1f; }
	if(sinput->keys[SDLK_z]) { globalAmbient += 0.05f; }
	if(sinput->keys[SDLK_h]) { if(globalAmbient > 0.f) globalAmbient -= 0.05f; }
	if(sinput->keys[SDLK_u]) { shininess += 0.5f; }
	if(sinput->keys[SDLK_j]) { if(shininess > 1.f) shininess -= 0.5f; }

}

void move()
{
	// move entities
	game.command();
	game.processTasks(elapsedTime);
	game.move(elapsedTime);
}

void draw()
{
	cxt->clearScreen();
	// sh.enable(true);
	cam.control(elapsedTime);

	cam.reset();
    //glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient );
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	//glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	cam.look();

	glEnable(GL_TEXTURE_2D);

	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();
	glTranslatef(lightPos[0],lightPos[1],lightPos[2]);
	drawCube(0.5f);
	glPopMatrix();

	game.drawMap();
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	game.drawEntities();
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	// sh.enable(false);

	game.drawQuads();

	glDisable(GL_TEXTURE_2D);
	cam.begin2d();

	glColor3f(1.f, 1.f, 1.f);

	if(sinput->keys[SDLK_F1])
		print_infos();
	
	if(game.areaSel)
	{
		glColor3f(1.f, 1.f, 1.f);
		glBegin(GL_LINE_STRIP);
		glVertex3i(game.startMouseSelX, game.startMouseSelY, -1.f);
		glVertex3i(game.endMouseSelX, game.startMouseSelY, -1.f);
		glVertex3i(game.endMouseSelX, game.endMouseSelY, -1.f);
		glVertex3i(game.startMouseSelX, game.endMouseSelY, -1.f);
		glVertex3i(game.startMouseSelX, game.startMouseSelY, -1.f);
		glEnd();
	}

	game.drawMinimap();
	game.draw2D();
	cam.end2d();

	if(!showPicking) cxt->flip();

	// print framerate
	if((cxt->secs() - before) >= 5.f)
	{
		printf("After 5s : %g FPS\n", frames/(cxt->secs()-before));
		frames = 0;
		before = cxt->secs();
	}
	else
		frames++;
}

void picking_system()
{
	cxt->clearScreen();
	cam.look();

	game.selection(cxt->height);

	if(showPicking) cxt->flip();
}


bool isExtSupported(const char* ext)
{
	const char* ext_list = (const char*)glGetString(GL_EXTENSIONS);
	return (strstr(ext_list, ext) != NULL);
}

int main(int nArg, char** pArgs)
{
    int width, height;
    bool fullscreen;
    FILE* res = fopen("Config/resolution.txt", "r");
    if(!res)
    {
        fprintf(stderr, "Fichier resolution.txt introuvable\n");
        return -1;
    }

    char line[128];
    fgets(line, 128, res);
    width = atoi(line);
    fgets(line, 128, res);
    height = atoi(line);
    fgets(line, 128, res);

    if(!strnicmp(line, "true", 4))
        fullscreen = true;
    else
        fullscreen = false;

    fclose(res);

    cxt = new invContext(width, height, fullscreen);
	game.setWindowResolution(width, height);

	// Init Context
	if(!cxt->create("Invasion", s_vfs->iconsandcursors("icons/icone.ico")))
	{
		fprintf(stderr, "Could not create the context\n");
		return -1;
	}

	glewInit();
	// extensions
	if(isExtSupported("GL_ARB_vertex_buffer_object"))
	{
		printf("VBO supported\n");
	}
	cxt->setBackground(0.f, 0.f, 0.1f);
	cxt->setFocus(false);

	// Cursor

	// Init Camera
	cam.init(cxt->width, cxt->height, 55.f);
	cam.setPos(Vec3f(0.f, 10.f, 10.f));

	// Define Flow
	flow.compensator = 0; // COMPENSATION;
	flow.setElapsedTime(&elapsedTime);

	flow.add(&flow.always, "checkEvent", &checkEvent);
	flow.add(&flow.always, "picking", &picking_system);
	flow.add(&flow.always, "move", &move);
	flow.add(&flow.always, "draw", &draw);

	// Load infos and models
	game.initScriptEngine();

	// init shaders
	// sh.load("vertex.vsh", "fragment.fsh");
	initLight();

	// launch timer
	before = cxt->secs();

	// run the main script
	game.circle = loadTexture(s_vfs->texture("circle.png"));
	game.runScripts();

	// V-Sync
	glPointSize(4.f);

	//------------------------------------------------
	// Launch the game
	//------------------------------------------------
	flow.loop();

	// Release circle texture
	glDeleteTextures(1, &game.circle);

	// unload the shaders
	// sh.unload();

	// Destroy the context
	cxt->destroy();
	delete cxt;

	// finish success
	return 0;
}
