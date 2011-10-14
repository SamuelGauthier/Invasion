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
GOBJModel* tree = 0;
GTerrain* trn = 0;
GFont* f = 0;
GControlManager* cm = 0;
float lightPosX = 1.f, lightPosY = 1.f, lightPosZ = 1.f, lightPosW = 0.f;
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

	cm = initControlManager(true);
	addSlideBar(cm, 0.f, 100, 50);

	// init Font
	f = loadFont("../Font/Test.fnt");

	tree = loadOBJ("../3DModels/tree.obj");
	setTexture(tree, "../Textures/tree_tex.tga");

	// init terrain
	trn = new GTerrain;
	addRelease(releaseTerrain,(void*)trn);
	setTexture(trn, "../Textures/grass.tga", 0);
	setTexture(trn, "../Textures/rock.tga", 1);
	trn->tex_size = 1.f/8.f;

	float* height = loadHeightMap("../Maps/heightmap.tga", trn->width);
	trn->cell = 3.f;
	trn->mntsize = 6.f;
	fillBuffers(trn, height);

	delete[] height;

	// init light
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
}

void OnRender(){

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

	int before = glutGet(GLUT_ELAPSED_TIME);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
	lookCamera(cam);
	
	// add an ambient light
	GLfloat ambientColor[] = {0.1f,0.1f,0.1f,1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	// add a spot light
	GLfloat lightColor0[] = {1.0f,1.0f,1.0f,1.f};
	GLfloat lightPos0[] = {lightPosX, lightPosY, lightPosZ,lightPosW};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);


	render(trn);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();
	glOrtho(0,800,600,0,-1,5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(show_fps)
		render(10,10,f,"FPS %d",fps);
	render(cm,f);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	glutSwapBuffers();

	acc += glutGet(GLUT_ELAPSED_TIME) - before;

	img_count++;
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
	glutTimerFunc(25, OnUpdate, 0);

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

	setMouse(cm, x, y, leftbutton);
}

void OnMousePassive(int x, int y){
	setCamera(cam, x, y);
	setMouse(cm, x, y, false);
}

void OnMouseActive(int x, int y){
	setCamera(cam, x, y);
	setMouse(cm, x, y, leftbutton);
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


