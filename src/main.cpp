#include <GL/glut.h>
#include "utils.h"
#include "camera.h"
#include "mesh.h"
#include "text.h"

// Variables globales
GFreeFlyCamera* cam = 0;
GTerrain* trn = 0;
GOBJModel* maison = 0;
GFont* f = 0;

bool key_state[256] = {false};
const int width = 800, height = 600;

// Prototypes de fonctions
void OnCreate();
void OnRelease();
void OnRender();
void OnKeyDown(uchar, int, int);
void OnKeyUp(uchar, int, int);
void OnMouse(int, int);
void OnUpdate(int);
void OnResize(int w,int h);
void OnTest();

void OnCreate(){
	cam = new GFreeFlyCamera;

	trn = generateTerrain(100, 10, 1);
	setTexture(trn, "../Textures/Texture_herbe.bmp", 8.f);
}

void OnRender(){
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cam->OnLook();
	render(trn);


	glutSwapBuffers();
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

	cam->OnKeyboard(&key_state[0]);

	glutPostRedisplay();
	glutTimerFunc(25, OnUpdate, 0);
}

void OnKeyDown(uchar key, int, int){
	key_state[key] = true;
}

void OnKeyUp(uchar key, int, int){
	key_state[key] = false;
}

void OnMouse(int x, int y){
	glutWarpPointer(width/2,height/2);

	cam->OnMouse(x-width/2,y-height/2);
}

void OnResize(int w, int h){
	glViewport(0,0,w,h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (double)w/(double)h, 1.0, 500.0);
}

int main(int argc, char** argv){
	/* initialisation de glut et création de la fenêtre */
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(-1,-1);
	glutInitWindowSize(width,height);
	glutCreateWindow("Test");

	// Initialisation des GObjets
	OnCreate();
	
	/* Initialisation d'OpenGL */
	glClearColor(0.0,0.0,0.0,0.0);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(width/2,height/2);
	glEnable(GL_DEPTH_TEST);
	
	/* On passe à une taille de 1 pixel */
	glPointSize(1.0);
	
	/* enregistrement des fonctions de rappel */
	glutDisplayFunc(OnRender);
	glutKeyboardFunc(OnKeyDown);
	glutKeyboardUpFunc(OnKeyUp);
	glutPassiveMotionFunc(OnMouse);
	glutReshapeFunc(OnResize);
	glutTimerFunc(25, OnUpdate, 0);

	/* entrée dans la boucle principale de glut */
	glutMainLoop();
}

void OnRelease(){
	releaseAll();
	delete cam;
}


