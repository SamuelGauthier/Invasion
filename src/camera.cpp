#include "camera.h"

GCamera* initCamera(const int mode, const Vec3f& pos, const float phi, const float theta)
{
	GCamera* cam = new GCamera;
	cam->sensitivity = 0.005f; cam->speed = 0.8f;

	setCamera(cam, pos, phi, theta);
	setMode(cam, mode);
	
	addRelease(releaseCamera, (void*)cam);
	return cam;
}

void lookCamera(const GCamera* cam)
{
	gluLookAt(cam->pos.x,cam->pos.y,cam->pos.z,cam->target.x,cam->target.y,cam->target.z,0.f,1.f,0.f);
}

void setVectors(GCamera* cam)
{
	Vec3f up(0.f,1.f,0.f);

	float corde = cosf(cam->theta);
	cam->forward.x = -corde * sinf(cam->phi);
	cam->forward.y = -sinf(cam->theta);
	cam->forward.z = -corde * cosf(cam->phi);

	cam->left = up.crossProduct(cam->forward);
	cam->left.normalize();


	cam->target = cam->pos + cam->forward;
}

void setCamera(GCamera* cam, const Vec3f& pos, const float phi, const float theta)
{
	cam->pos = pos;
	cam->phi = phi; cam->theta = theta;

	setVectors(cam);
}


void setCamera(GCamera* cam, int x, int y)
{
	if(cam->mode == freefly_camera)
	{
		glutWarpPointer(400,300);
		cam->phi -= (float)(x-400)* cam->sensitivity;
		cam->theta += (float)(y-300)* cam->sensitivity;
		
		if(cam->theta > M_PI/2.f - 0.01f)
			cam->theta = M_PI/2.f - 0.01f;
		if(cam->theta < -M_PI/2.f + 0.01f)
			cam->theta = -M_PI/2.f + 0.01f;
	}

	setVectors(cam);
}

void setCamera(GCamera* cam,const bool* key)
{
	if(cam->mode == freefly_camera)
	{
		if(key['w'])
			cam->pos += cam->forward*cam->speed;
		if(key['s'])
			cam->pos -= cam->forward*cam->speed;
		if(key['a'])
			cam->pos += cam->left*cam->speed;
		if(key['d'])
			cam->pos -= cam->left*cam->speed;
		cam->target = cam->pos + cam->forward;
	}
}

void releaseCamera(void* cam)
{	
	GCamera* tmp = (GCamera*)cam;
	delete tmp;
}

void setMode(GCamera* cam,const int mode)
{
	cam->mode = mode;

	if(mode == freefly_camera)
	{
		glutSetCursor(GLUT_CURSOR_NONE);
	}

	if(mode == fixed_camera || mode == strategy_game_camera)
	{
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}
	
	glutWarpPointer(400,300);
}
