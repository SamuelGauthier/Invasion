#define FPS 60
#define WIDTH_WINDOW 800
#define HEIGHT_WINDOW 600

#include <GL/glut.h>
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
void OnCreate();
void OnRelease();
void OnRender();
void OnRender2D();
void OnKeyDown(uchar, int, int);
void OnKeyUp(uchar, int, int);
void OnMouse(int, int, int, int);
void OnMousePassive(int, int);
void OnMouseActive(int, int);
void OnUpdate(int);
void OnResize(int w,int h);
void OnTest();

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

	planeTerrain(trn, 2);
	setTexture(trn, "../Textures/grass.tga",0);
	trn->cell = 10.f;
	fillBuffers(trn); 
	
	// init state
	glEnable(GL_DEPTH_TEST);
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

	glutSwapBuffers();
}

void OnRender2D()
{
	render(cmd, f);
}

void OnUpdate(int){

	// Keyboard management
	if(key_state[27]){
		OnRelease();
		exit(0);
	}

	if(key_state['f'])
	{	
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
	}

	if(key_state['g'])
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_POINT);
	}

	if(key_state['h'])
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
	}


	setCamera(cam, &key_state[0]);
	cam->pos.x += 0.02f;
	setVectors(cam);

	glutPostRedisplay();
	glutTimerFunc(1000/FPS, OnUpdate, 0);
}

void OnKeyDown(uchar key, int, int){
	key_state[key] = true;

	if(key == 'q')
	{
		toggleConsole(cmd);
	}
}

void OnKeyUp(uchar key, int, int){
	key_state[key] = false;
}

void OnMouse(int button, int state, int x, int y)
{	
	/*
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		leftbutton = true;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		leftbutton = false;
	*/
}

void OnMousePassive(int x, int y){
	setCamera(cam, x, y);
}

void OnMouseActive(int x, int y){
	setCamera(cam, x, y);
}

void OnResize(int w, int h){
	glViewport(0,0,w,h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (double)w/(double)h,1.f, 500.0);
}

int main(int argc, char** argv){
	/* initialisation de glut et création de la fenêtre */
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(-1,-1);
	glutInitWindowSize(WIDTH_WINDOW,HEIGHT_WINDOW);
	glutCreateWindow("GEngine");

	// Initialisation des GObjets
	OnCreate();
	
	/* Initialisation d'OpenGL */
	glClearColor(0.f,0.f,0.1f,0.f);
	
	/* On passe à une taille de 1 pixel */
	glPointSize(1.0);
	
	/* enregistrement des fonctions de rappel */
	glutDisplayFunc(OnRender);
	glutKeyboardFunc(OnKeyDown);
	glutKeyboardUpFunc(OnKeyUp);
	glutMouseFunc(OnMouse);
	glutPassiveMotionFunc(OnMousePassive);
	glutMotionFunc(OnMouseActive);
	glutReshapeFunc(OnResize);
	glutTimerFunc(1000/FPS, OnUpdate, 0);

	/* entrée dans la boucle principale de glut */
	glutMainLoop();

}

void OnRelease(){
	releaseAll();
}


