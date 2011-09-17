#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include <math.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

class GCamera 
{
public:
	GCamera(const Vec3f& _pos = Vec3f(0.f,0.f,0.f));

	virtual void setPos(const Vec3f& _pos);

	void setSensitivity(float s);
	void setSpeed(float s);

	virtual void OnLook() const;


	virtual ~GCamera();


	float sensitivity, speed;

	Vec3f pos;
	Vec3f forward;
	Vec3f left;
	Vec3f target;
	float phi,theta;

	virtual void setVec3fs();
};

class GFreeFlyCamera : public GCamera
{
public:
	GFreeFlyCamera(const Vec3f& _pos = Vec3f(0.f,0.f,0.f));

	void OnMouse(int x, int y);
	void OnKeyboard(bool* key);

	virtual ~GFreeFlyCamera();
};

/*
class GStrategyCamera 
{
public:
	GStrategyCamera(const Vec3f& _pos = Vec3f(0.f,0.f,0.f));

	void Mouse(int x, int y);
	void OnKeyboard(bool* key);
	virtual ~GStrategyCamera();
};
*/
#endif
