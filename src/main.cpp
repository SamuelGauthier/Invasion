#include <GL/glut.h>
#include <unistd.h>
#include "utils.h"
#include "camera.h"
#include "mesh.h"
#include "text.h"
#include "terrain.h"
#include "ui.h"

// Variables globales
GCamera* cam = 0;
GTerrain* trn = 0;
GFont* f = 0;
int fps = 0.f;
int acc = 0, img_count = 0, first_wait_count = 10, wait_count = 0;
int before = 0;
bool show_fps = true;
int wait_show_fps = 0;
int delay = 0;
bool leftbutton = false;

bool key_state[256] = {false};
const int width = 800, height = 600;

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

	// init terrain
	trn = new GTerrain;
	addRelease(releaseTerrain,(void*)trn);
	setTexture(trn, "../Textures/grass.tga", 0);
	trn->tex_size = 1.f/8.f;

	float* height = planeTerrain(&trn->width, 50);
	trn->cell = 3.f;
	trn->mntsize = 6.f;
	fillBuffers(trn, height);

	delete[] height;

	// init light
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
}

void OnRender(){

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	int before = glutGet(GLUT_ELAPSED_TIME);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
	lookCamera(cam);
	
	glPushMatrix();
	glColor3f(0.2f,0.7f,0.1f);
	glTranslatef(0.f,5.f,0.f);
	glutSolidCube(2.f);
	glPopMatrix();

	render(trn);

	OnRender2D();
	glutSwapBuffers();

	acc += glutGet(GLUT_ELAPSED_TIME) - before;
	img_count++;
}

void OnRender2D()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	glOrtho(0,800,600,0,-1,5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(show_fps)
		render(10,10,f,"FPS %d",fps);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
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


	if(key_state['q'] && !delay)
	{
		show_fps = !show_fps;
		delay = 10;
	}
	
	if(key_state['e'] && !delay)
	{
		if(cam->mode == freefly_camera)
			setMode(cam,fixed_camera);
		else
			setMode(cam,freefly_camera);
		delay = 10;
	}

	setCamera(cam, &key_state[0]);

	glutPostRedisplay();
	glutTimerFunc(15, OnUpdate, 0);

	// Calcul fps
	first_wait_count--;
	if(!first_wait_count)
	{
		acc = 0;
		img_count = 0;
		wait_count = 20;
	}
	
	wait_count--;
	if(!wait_count)
	{
		fps = (int)(1.f/((((float)acc)/1000.f)/(float)img_count));
		wait_count = 10;
	}

	if(delay){delay--;}
}

void OnKeyDown(uchar key, int, int){
	key_state[key] = true;
}

void OnKeyUp(uchar key, int, int){
	key_state[key] = false;
}

void OnMouse(int button, int state, int x, int y)
{	
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		leftbutton = true;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		leftbutton = false;
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
	glutInitWindowSize(width,height);
	glutCreateWindow("GEngine");

	// Initialisation des GObjets
	OnCreate();
	
	/* Initialisation d'OpenGL */
	glClearColor(0.0,0.0,0.0,0.0);
	
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
	glutTimerFunc(25, OnUpdate, 0);

	/* entrée dans la boucle principale de glut */
	setTimer(10000);
	glutMainLoop();

}

void OnRelease(){
	releaseAll();
}


