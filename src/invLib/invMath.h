#ifndef INVMATH_H
#define INVMATH_H
#define F_PI 3.14593f
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <math.h>

//------------------------------------------------
// Vec2f
//------------------------------------------------
struct Vec2f 
{
	float x,y;
	Vec2f(float _x=0.f, float _y=0.f);
}; /* Vec2f */

//------------------------------------------------
// Vec3f
//------------------------------------------------
struct Vec3f
{
	float x,y,z;

	Vec3f(float _x=0.f, float _y=0.f, float _z=0.f);

	Vec3f operator+(const Vec3f& a) const;
	Vec3f& operator+=(const Vec3f& a);
	Vec3f operator-(const Vec3f& a) const;
	Vec3f& operator-=(const Vec3f& a);
	Vec3f operator*(const float n) const;
	Vec3f& operator*=(const float n);
	Vec3f operator/(const float n) const;
	Vec3f& operator/=(const float n);
	Vec3f& operator=(const float n);
	Vec3f& operator=(const Vec3f& v);
	Vec3f crossProduct(const Vec3f& v) const;
	void normalise();
	float getLengthInv() const;
};

//------------------------------------------------
// Qua4f
//------------------------------------------------
struct Qua4f
{
	float x,y,z,w;
	Qua4f(float _x=0.f, float _y=0.f, float _z=0.f, float _w=0.f);
	void normalise();
	Qua4f getConjugate();
	Qua4f operator*(const Qua4f& q) const;
	Vec3f operator*(const Vec3f& v);
	void eulerAngle(float pitch, float yaw, float roll);
	void getMatrix(float* m);
};

//------------------------------------------------
// AABB Box
//------------------------------------------------
struct Box
{
	float x, y, z;
};

//------------------------------------------------
// Camera
//------------------------------------------------
namespace Camera 
{
	extern Vec3f pos;
	extern float updown, leftright;
	extern Vec3f dir;
	void lookFPS();
	void mouseFPS(float ElapsedTime);
	void lookRTS();
	void mouseRTS(float ElapsedTime);
	void lookFixed();
	void mouseFixed(float);
	void switchCameraView(char* args[], void* data);
} /* Camera */

//------------------------------------------------
// Array2D - for now only the square matrices
//------------------------------------------------
namespace Array2D 
{
	float** fill(void* pixels, const Uint8 bpp, const int length);
	float** plane(const int length);
	float** random(const int l, const int max=50, const int min=0);
	float interpolate(float a, float b, float percent);
	void linear_interpolation(float** array, const int size, const int freq);
} /* Array2D */

//------------------------------------------------
// Math
//------------------------------------------------
namespace Math 
{
	float inv_sqrt(float a);
	float absf(float x);
	float intersect(Box* b, Vec3f* pos, Vec3f* dir, Vec3f* origin);
} /* Math */

#endif
