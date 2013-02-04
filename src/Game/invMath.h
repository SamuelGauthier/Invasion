#ifndef INVMATH_H
#define INVMATH_H
#define F_PI 3.1416f
#include <math.h>

//------------------------------------------------
// Vec2f
//------------------------------------------------
struct Vec2f 
{
	float x,y;
	Vec2f(float _x=0.f, float _y=0.f);

	Vec2f operator+(const Vec2f& a) const;
	Vec2f& operator+=(const Vec2f& a);
	Vec2f operator-(const Vec2f& a) const;
	Vec2f& operator-=(const Vec2f& a);
	Vec2f operator*(const float n) const;
	Vec2f& operator*=(const float n);
	Vec2f operator/(const float n) const;
	Vec2f& operator/=(const float n);
	Vec2f& operator=(const float n);
	Vec2f& operator=(const Vec2f& v);
	void normalise();
	float getLengthInv() const;


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

float squaredDistance(Vec3f* pos1, Vec3f* pos2);
float squaredDistance(Vec2f* pos1, Vec2f* pos2);

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
// Math
//------------------------------------------------
namespace Math 
{
	float inv_sqrt(float a);
	float absf(float x);
	int next_p2(int a);
	float maxf(float a, float b);
	float minf(float a, float b);
} /* Math */

//------------------------------------------------
// Small Algos
//------------------------------------------------
void bubblesort(int* array, int count);


#endif
