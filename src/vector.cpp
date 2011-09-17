#include "vector.h"
#include "utils.h"

Vec2f::Vec2f(float _x, float _y) :
	x(_x), y(_y)
{
}

Vec2f::~Vec2f()
{
}

Vec3f::Vec3f(float _x, float _y, float _z) :
	x(_x), y(_y), z(_z)
{
}

Vec3f Vec3f::operator+(const Vec3f& a) const
{
	Vec3f copie(a);
	copie += *this;
	return copie;
}

Vec3f::~Vec3f()
{
}

Vec3f& Vec3f::operator+=(const Vec3f& a)
{
	x+=a.x;y+=a.y;z+=a.z;
	return *this;
}

Vec3f Vec3f::operator-(const Vec3f& a) const
{
	Vec3f copie(*this);
	copie-=a;
	return copie;
}

Vec3f& Vec3f::operator-=(const Vec3f& a)
{
	x-=a.x;y-=a.y;z-=a.z;
	return *this;
}

Vec3f Vec3f::operator*(const float n) const
{
	return Vec3f(*this)*=n;
}

Vec3f& Vec3f::operator*=(const float n)
{
	x*=n;y*=n;z*=n;
	return *this;
}

Vec3f Vec3f::operator/(const float n) const
{
	return Vec3f(*this)/=n;
}

Vec3f& Vec3f::operator/=(const float n)
{
	x/=n;y/=n;z/=n;
	return *this;
}

Vec3f& Vec3f::operator=(const Vec3f& v)
{
	x = v.x;
	y = v.y;
	z = v.z;

	return *this;
}

Vec3f& Vec3f::operator=(const float n)
{
	x = n;
	y = n;
	z = n;

	return *this;
}

Vec3f Vec3f::crossProduct(const Vec3f& v) const
{
	return Vec3f((y * v.z)-(z * v.y),
				  (z * v.x)-(x * v.z),
				  (x * v.y)-(y * v.x));
}

void Vec3f::normalize()
{
	float lengthInv = getLengthInv();

	x = x*lengthInv;
	y = y*lengthInv;
	z = z*lengthInv;
}

// Get the inverse 
float Vec3f::getLengthInv() const
{
	return Cabs(Csqrt(x*x + y*y + z*z));
}
