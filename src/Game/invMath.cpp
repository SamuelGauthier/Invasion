#include "invMath.h"

//------------------------------------------------
// Math
//------------------------------------------------
float Math::inv_sqrt(float x)
{
	long i;
    float x2, y;
	const float threehalfs = 1.5f;

	x2 = x * 0.5f;
	y  = x;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );

	return y;	
}

float Math::absf(float x)
{
	if(x < 0.f)
		return -x;
	return x;
}

int Math::next_p2(int a)
{
	int result = 1;
	while(result < a){ result <<= 1; }
	return result;
}

float Math::maxf(float a, float b)
{
	return (a > b) ? a : b;
}

float Math::minf(float a, float b)
{
	return (a <= b) ? a : b;
}

//------------------------------------------------
// 2D & 3D Vectors
//------------------------------------------------
Vec2f::Vec2f(float _x, float _y) : x(_x), y(_y)
{
}

Vec2f Vec2f::operator+(const Vec2f& a) const
{
	Vec2f copie(a);
	copie += *this;
	return copie;
}

Vec2f& Vec2f::operator+=(const Vec2f& a)
{
	x+=a.x;y+=a.y;
	return *this;
}

Vec2f Vec2f::operator-(const Vec2f& a) const
{
	Vec2f copie(*this);
	copie-=a;
	return copie;
}

Vec2f& Vec2f::operator-=(const Vec2f& a)
{
	x-=a.x;y-=a.y;
	return *this;
}

Vec2f Vec2f::operator*(const float n) const
{
	return Vec2f(*this)*=n;
}

Vec2f& Vec2f::operator*=(const float n)
{
	x*=n;y*=n;
	return *this;
}

Vec2f Vec2f::operator/(const float n) const
{
	return Vec2f(*this)/=n;
}

Vec2f& Vec2f::operator/=(const float n)
{
	x/=n;y/=n;
	return *this;
}

Vec2f& Vec2f::operator=(const Vec2f& v)
{
	x = v.x;
	y = v.y;

	return *this;
}

Vec2f& Vec2f::operator=(const float n)
{
	x = n;
	y = n;

	return *this;
}

void Vec2f::normalise()
{
	float lengthInv = getLengthInv();

	x = x*lengthInv;
	y = y*lengthInv;
}

// Get the inverse 
float Vec2f::getLengthInv() const
{
	return Math::absf(Math::inv_sqrt(x*x + y*y));
}


Vec3f::Vec3f(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

Vec3f Vec3f::operator+(const Vec3f& a) const
{
	Vec3f copie(a);
	copie += *this;
	return copie;
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

void Vec3f::normalise()
{
	float lengthInv = getLengthInv();

	x = x*lengthInv;
	y = y*lengthInv;
	z = z*lengthInv;
}

// Get the inverse 
float Vec3f::getLengthInv() const
{
	return Math::absf(Math::inv_sqrt(x*x + y*y + z*z));
}

float squaredDistance(Vec3f* pos1, Vec3f* pos2)
{
	float dx = pos2->x - pos1->x;
	float dy = pos2->y - pos1->y;
	float dz = pos2->z - pos1->z;

	return (dx * dx + dy * dy + dz * dz);
}

float squaredDistance(Vec2f* pos1, Vec2f* pos2)
{
	float dx = pos2->x - pos1->x;
	float dy = pos2->y - pos1->y;

	return (dx * dx + dy * dy);
}
//------------------------------------------------
// Qua4f
// "Copied" from http://content.gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation
//------------------------------------------------
Qua4f::Qua4f(float _x, float _y, float _z, float _w)
{
	x = _x; y = _y; z = _z; w = _w;
}

void Qua4f::normalise()
{
	float mag = 1.f/Math::inv_sqrt(w*w + x*x + y*y + z*z);
	w /= mag;
	x /= mag;
	y /= mag;
	z /= mag;
}

Qua4f Qua4f::getConjugate()
{
	return Qua4f(-x, -y, -z, w);
}

Qua4f Qua4f::operator*(const Qua4f& q) const
{
	return Qua4f	(w * q.x + x * q.w + y * q.z - z * q.y,
	                 w * q.y + y * q.w + z * q.x - x * q.z,
	                 w * q.z + z * q.w + x * q.y - y * q.x,
	                 w * q.w - x * q.x - y * q.y - z * q.z);
}

Vec3f Qua4f::operator*(const Vec3f& v) 
{
	Vec3f vn = v;
	vn.normalise();

	Qua4f vecQuat, resQuat;
	vecQuat.x = vn.x;
	vecQuat.y = vn.y;
	vecQuat.z = vn.z;
	vecQuat.w = 0.f;

	Qua4f conj = getConjugate();
	resQuat = vecQuat * conj;
	resQuat = *this * resQuat;

	return (Vec3f(resQuat.x, resQuat.y, resQuat.z));
}

void Qua4f::eulerAngle(float pitch, float yaw, float roll)
{
	float p = pitch * (F_PI/180.f) /2.f;
	float y = yaw * (F_PI/180.f) /2.f;
	float r = roll * (F_PI/180.f) /2.f;

	float sinp = sinf(p);
	float siny = sinf(y);
	float sinr = sinf(r);
	float cosp = cosf(p);
	float cosy = cosf(y);
	float cosr = cosf(r);
	
	this->x = sinr * cosp * cosy - cosr * sinp * siny;
	this->y = cosr * sinp * cosy + sinr * cosp * siny;
	this->z = cosr * cosp * siny - sinr * sinp * cosy;
	this->w = cosr * cosp * cosy + sinr * sinp * siny;

	normalise();
}

void Qua4f::getMatrix(float* m)
{
	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;
	
	m[0]=1.f-2.f*(y2+z2); m[1]=2.f*(xy-wz); m[2]=2.f*(xz+wy); m[3]=0.f;
	m[4]=2.f*(xy+wz); m[5]=1.f-2.f*(x2+z2); m[6]=2.f*(yz-wx); m[7]=0.f;
 	m[8]=2.f*(xz-wy); m[9]=2.f*(yz+wx); m[10]=1.f-2.f*(x2+y2); m[11]=0.f;
	m[12]=0.f; m[13]=0.f; m[14]=0.f; m[15]=1.f;
}

//------------------------------------------------
// Little algorithms
//------------------------------------------------
void bubblesort(int* array, int count)
{
	bool swap = true;
	while(swap)
	{
		swap = false;
		for(int i=0;i<count-1;i++)
		{
			if(array[i] > array[i+1])
			{
				int tmp = array[i];
				array[i] = array[i+1];
				array[i+1] = tmp;
				swap = false;
			}
		}
	}
}

