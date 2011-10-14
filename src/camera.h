#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include <math.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "garbage_collector.h"

enum CAMERA_TYPE
{
	unknown = 0,
	fixed_camera,
	freefly_camera,
	strategy_game_camera
};

struct GCamera
{
	float sensitivity, speed;

	Vec3f pos;
	Vec3f forward;
	Vec3f left;
	Vec3f target;
	float phi, theta;

	int mode;
};

GCamera* initCamera(const int mode,const Vec3f& pos=Vec3f(0.f,0.f,0.f),const float phi=0.f,const float theta=0.f);
void setMode(GCamera* cam,const int mode);
void setCamera(GCamera* cam, int x, int y);
void setCamera(GCamera* cam, const bool* key);
void setCamera(GCamera* cam,const Vec3f& pos,const float phi,const float theta);
void lookCamera(const GCamera* cam);
void setVectors(GCamera* cam);
void releaseCamera(void* cam);

#endif
