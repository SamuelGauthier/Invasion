#include "invTex.h"
#include "invTalk.h"
#include "invMath.h"
#include "invBase.h"

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

float Math::intersect(Box* bound, Vec3f* pos, Vec3f* dir, Vec3f* origin)
{
	static float a[6];
	bool cut = false;
	a[0] = (bound->x + pos->x - origin->x)/dir->x;
	a[1] = (-bound->x + pos->x - origin->x)/dir->x;
	a[2] = (bound->y + pos->y - origin->y)/dir->y;
	a[3] = (0.f + pos->y - origin->y)/dir->y;
	a[4] = (bound->z + pos->z - origin->z)/dir->z;
	a[5] = (-bound->z + pos->z - origin->z)/dir->z;

	float min = 0.f, max = 0.f, b, c;
	for(int i=0;i<6;i+=2){
		b = (a[i] < a[i+1]) ? a[i] : a[i+1];
		c = (a[i] >= a[i+1]) ? a[i] : a[i+1];
		if(!i){
			min = b; max = c;
		}
		else{
			min = (b > min) ? b : min; max = (c < max) ? c : max;
		}
	}

	return max-min;
}

//------------------------------------------------
// Vector
//------------------------------------------------
Vec2f::Vec2f(float _x, float _y) : x(_x), y(_y)
{
}

Vec3f::Vec3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
{
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

//------------------------------------------------
// Qua4f
// Copied from http://content.gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation
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
// Camera
//------------------------------------------------
Vec3f Camera::pos = Vec3f(0.f, 4.f, 5.f), Camera::dir = Vec3f(0.0f, 1.f, 0.f);
float Camera::updown = 0.f, Camera::leftright;

void Camera::lookFPS()
{
	gluLookAt(pos.x, pos.y, pos.z,
    			pos.x + dir.x, pos.y + dir.y, pos.z + dir.z,
    			0.f, 1.f, 0.f);
}

void Camera::mouseFPS(float ElapsedTime)
{	
	leftright -= (float)(Input::mousex - Game::width/2) / 200.f;
	updown -= (float)(Input::mousey - Game::height/2) / 200.f;

	if(updown > F_PI/2.f - 0.01f)
		updown = F_PI/2.f - 0.01f;
	if(updown < -F_PI/2.f + 0.01f)
		updown = -F_PI/2.f + 0.01f;

	dir.x = -cosf(updown) * sinf(leftright);
	dir.y = sinf(updown);
	dir.z = -cosf(updown) * cosf(leftright);

	if(Input::key['w'])
		pos += dir * ElapsedTime * 0.01f;
	if(Input::key['s'])
		pos -= dir * ElapsedTime * 0.01f;
	if(Input::key['d'] || Input::key['a'])
	{
		Vec3f up(0.f, 1.f, 0.f);
		Vec3f left = up.crossProduct(dir);
		left.normalise();

		if(Input::key['d'])
			pos -= left * ElapsedTime * 0.01f;

		if(Input::key['a'])
			pos += left * ElapsedTime * 0.01f;
	}
	SDL_WarpMouse((Uint16)Game::width/2, (Uint16)Game::height/2);
}

void Camera::lookRTS()
{
	gluLookAt(	pos.x, pos.y, pos.z + 3.f,
    			pos.x, 0.f, pos.z,
    			0.f, 1.f, 0.f);

}

void Camera::mouseRTS(float ElapsedTime)
{
	if(Input::mousex < 10)
		pos.x -= ElapsedTime * 0.01f;
	else if(Input::mousex > Game::width - 10)
		pos.x += ElapsedTime * 0.01f;
	if(Input::mousey < 10)
		pos.z -= ElapsedTime * 0.01f;
	else if(Input::mousey > Game::height - 10)
		pos.z += ElapsedTime * 0.01f;
}

void Camera::lookFixed()
{
	gluLookAt( pos.x + 1.f, pos.y, pos.z,
			   dir.x + 1.f, dir.y, dir.z,
			   0.f, 1.f, 0.f);
}

void Camera::mouseFixed(float ElapsedTime)
{
	if(Input::key['w'])
		pos.y += 0.01f * ElapsedTime;
	if(Input::key['s'])
		pos.y -= 0.01f * ElapsedTime;
	if(Input::key['a'])
		Game::pEntityList->pProList->dir.x -= 2.f;
	if(Input::key['d'])
		Game::pEntityList->pProList->dir.x += 2.f;
}

void Camera::switchCameraView(char* args[], void* data) /* FIXME args const */
{
	if(!strcasecmp(args[0], "fps"))
	{
		dir = pos = Vec3f(0.f, 0.f, 0.f); 
		updown = 0.f; leftright = 0.f;
		SDL_ShowCursor(SDL_DISABLE);
		SDL_WarpMouse((Uint16)Game::width/2, (Uint16)Game::height/2);

		Game::bindCamera(lookFPS, mouseFPS);
	}

	else if(!strcasecmp(args[0], "rts"))
	{
		pos = Vec3f(0.f, 5.f, 0.f);

		SDL_ShowCursor(SDL_ENABLE);
		SDL_WarpMouse((Uint16)Game::width/2, (Uint16)Game::height/2);
		Game::bindCamera(lookRTS, mouseRTS);
	}

	else
	{
		printf("error switchCameraView\n"); /* FIXME print a message in the console */
	}
}

//------------------------------------------------
// Array2D
//------------------------------------------------
float** Array2D::fill(void* pixels, const Uint8 bpp, const int length)
{
	float** array = new float*[length];	
	for(int i=0;i<length;i++)
		array[i] = new float[length];
	
	for(int i=0;i<length;i++)
	{
		for(int j=0;j<length;j++)
		{
			array[i][j] = (float)((((Uint32*)pixels)[(i * length + j)] & 0xFF00) >> 8)* 20.f/256.f;
		}
	}
	return array;
}

float** Array2D::plane(const int length)
{	
	float** array = new float*[length];	
	for(int i=0;i<length;i++)
		array[i] = new float[length];
	
	for(int i=0;i<length;i++)
	{
		for(int j=0;j<length;j++)
			array[i][j] = 0.f;
	}

	return array;
}

float** Array2D::random(const int length, const int max, const int min)
{
	float** array = new float*[length];
	for(int i=0;i<length;i++)
		array[i] = new float[length];
	for(int i=0;i<length;i++){
		for(int j=0;j<length;j++)
			array[i][j] = (float)((rand()%max) + min);
	}

	linear_interpolation(array, length, 3);

	return array;
}

float Array2D::interpolate(float a, float b, float percent)
{
	if(percent==0.f)
	    return a;
	if(percent==1.f)
	    return b;
	
	float v1 = 3.f * powf(1.f-percent, 2.f) - 2.f*powf(1.f-percent, 3.f);
	float v2 = 3.f * powf(percent, 2.f) - 2.f *powf(percent, 3.f);
	
	return a*v1 + b*v2;
}

void Array2D::linear_interpolation(float** array, const int size, const int freq)
{
	
	float chunk = (float)(size-1)/(float)(freq+1);
	int k0 = 0, k1 = 0;
	float b1 = 0.f, b2 = 0.f, b3 = 0.f, b4 = 0.f;
	for(int i=0;i<freq+1;i++)
	{	
		k0 = k1;
		k1 = (int)(chunk * (float)(i+1));
		int l0 = 0, l1 = 0;
		for(int j=0;j<freq+1;j++)
		{	
			/* interpolation */
			l0 = l1;
			l1 = (int)(chunk * (float)(j+1));
			b1 = array[k0][l0]; b2 = array[k0][l1]; b3 = array[k1][l0]; b4 = array[k1][l1];
			
			for(int k=0;k<k1-k0+1;k++)
			{
				for(int l=0;l<l1-l0+1;l++)
				{
					float percentx = (float)k/(float)(k1-k0);
					float percentz = (float)l/(float)(l1-l0);

					float a = interpolate(b1, b3, percentx);
					float b = interpolate(b2, b4, percentx);
					array[k0+k][l0+l] = interpolate(a, b, percentz);
				}
			}
		}
	}
}
