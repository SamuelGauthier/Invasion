#include "camera.h"

GCamera::GCamera(const Vec3f& _pos)
{
	pos = _pos;
	sensitivity = 0.005f;
	speed = 0.8f;
	phi = theta = 0.f;

	setVec3fs();
	
}

GCamera::~GCamera()
{
}

void GCamera::setPos(const Vec3f& _pos)
{
	pos = _pos;
	target = pos + forward;
}

void GCamera::OnLook() const
{
	gluLookAt(pos.x,pos.y,pos.z,target.x,target.y,target.z,0.f,1.f,0.f);
}

void GCamera::setSensitivity(float s) { sensitivity = s; }
void GCamera::setSpeed(float s) { speed = s; }

void GCamera::setVec3fs()
{
	Vec3f up(0.f,1.f,0.f);

	float corde = cos(theta);
	forward.x = -corde * sin(phi);
	forward.y = -sin(theta);
	forward.z = -corde * cos(phi);

	left = up.crossProduct(forward);
	left.normalize();


	target = pos + forward;
}

GFreeFlyCamera::GFreeFlyCamera(const Vec3f& _pos):
	GCamera(_pos)
{
}

GFreeFlyCamera::~GFreeFlyCamera()
{
}
	
void GFreeFlyCamera::OnMouse(int xrel, int yrel)
{
	phi -= (float)xrel  * sensitivity;
	theta += (float)yrel * sensitivity;
	
	if(theta > M_PI/2.f - 0.01f)
		theta = M_PI/2.f - 0.01f;
	if(theta < -M_PI/2.f + 0.01f)
		theta = -M_PI/2.f + 0.01f;
	
	setVec3fs();
}

void GFreeFlyCamera::OnKeyboard(bool* key)
{
	if(key['w'])
		pos += forward*speed;
	if(key['s'])
		pos -= forward*speed;
	if(key['a'])
		pos += left*speed;
	if(key['d'])
		pos -= left*speed;
	target = pos + forward;
}

/*
GStrategyCamera::GStrategyCamera(const Vec3f& _pos)
{
	pos = _pos;
	forward = Vec3f(0.f,0.f,-1.f);
	target = Vec3f(0.f,-1.f,-1.f);
	left = Vec3f(-1.f,0.f,0.f);
}

void GStrategyCamera::OnMouse(int x, int y)
{

}
*/
