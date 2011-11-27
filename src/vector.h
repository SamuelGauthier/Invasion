#ifndef VECTOR_H
#define VECTOR_H

class Vec3f
{
public:
	Vec3f(float x=0.f, float y=0.f, float z=0.f);

	Vec3f operator+(const Vec3f&) const;
	Vec3f& operator+=(const Vec3f&);

	Vec3f operator-(const Vec3f&) const;
	Vec3f& operator-=(const Vec3f&);

	Vec3f operator*(const float) const;
	Vec3f& operator*=(const float);

	Vec3f operator/(const float) const;
	Vec3f& operator/=(const float);

	Vec3f& operator=(const Vec3f&);
	Vec3f& operator=(const float);

	Vec3f crossProduct(const Vec3f&) const;
	void normalize();
	float getLengthInv() const;

	~Vec3f();

	float x,y,z;
};

class Vec2f 
{
public:
	Vec2f(float x=0.f, float y=0.f);
	~Vec2f();

	float x,y;
};

struct Vec4f
{
	float x,y,z,w;
};

#endif
