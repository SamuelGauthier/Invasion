#include "invScene.h"
#include "invInput.h"

//------------------------------------------------
// invCamera
//		^
//		| y
// 		|
//      |_________>
// 	   /       x
//    / 
//   / z
//  v
//------------------------------------------------
invCamera::invCamera(CAMERA_MODE m)
{
	pos = Vec3f(0.f, 0.f, 0.f);
	ahoriz = 0.f; avert = 0.f;
	mode = m;
	computeVectors();
	sensibility = 0.01f;
	speed = Vec3f(0.f, 0.f, 0.f);
}

invCamera::~invCamera()
{
}

void invCamera::setPos(Vec3f p)
{
	pos = p;
}

void invCamera::setAngles(float h, float v)
{	
	ahoriz = h * (F_PI/180.f);
	avert = v * (F_PI/180.f);

	computeVectors();
}

void invCamera::setHeight(float height)
{
	pos.y = height;
}

void invCamera::init(int w, int h, float fov, float fmin, float fmax)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective((GLdouble)fov, (GLdouble)w/(GLdouble)h, (GLdouble)fmin, (GLdouble)fmax);

	if(mode == FPS_CAM)
		SDL_ShowCursor(SDL_DISABLE);

	width = w;
	height = h;

	SDL_PumpEvents();
	SDL_WarpMouse((Uint16)w/2, (Uint16)h/2);
}

void invCamera::control(float ElapsedTime)
{
	if(mode == FPS_CAM)
	{
		ahoriz += (float)(sinput->mousex - width/2) * sensibility;
		avert  -= (float)(sinput->mousey - height/2) * sensibility;

		if(avert > F_PI/2.f - 0.01f)
			avert = F_PI/2.f - 0.01f;
		if(avert < -F_PI/2.f + 0.01f)
			avert = -F_PI/2.f + 0.01f;

		computeVectors();
		SDL_WarpMouse((Uint16)width/2, (Uint16)height/2);

		if(sinput->keys['w'])
			pos += dir * ElapsedTime * sensibility * 1000.f;
		if(sinput->keys['s'])
			pos -= dir * ElapsedTime * sensibility * 1000.f;
		if(sinput->keys['d'] || sinput->keys['a'])
		{
			Vec3f up(0.f, 1.f, 0.f);
			Vec3f left = up.crossProduct(dir);
			left.normalise();

			if(sinput->keys['d'])
				pos -= left * ElapsedTime * sensibility * 1000.f;

			if(sinput->keys['a'])
				pos += left * ElapsedTime * sensibility * 1000.f;

		}
	}

	if(mode == RTS_CAM)
	{
		float step = 1000.f * sensibility * ElapsedTime;
		if(10 > sinput->mousex)
		{
			if(speed.x > -1.f) speed.x -= 0.01f;
		}

		else if(width - 10 < sinput->mousex)
		{
			if(speed.x < 1.f) speed.x += 0.01f;
		}

		else
		{
			speed.x += -speed.x/40.f;
		}

		if(10 > sinput->mousey)
		{
			if(speed.z > -1.f) speed.z -= 0.01f;
		}

		else if(height - 10 < sinput->mousey)
		{
			if(speed.z < 1.f) speed.z += 0.01f;
		}

		else
		{
			speed.z += -speed.z/40.f;
		}

		pos.x += speed.x * step;
		pos.z += speed.z * step;
	}
}

void invCamera::reset()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void invCamera::look()
{
	reset();
	gluLookAt(pos.x, pos.y, pos.z,
			  pos.x + dir.x, pos.y + dir.y, pos.z + dir.z,
			  0.f, 1.f, 0.f);
}

void invCamera::lookAt(float tx, float ty, float tz)
{
	reset();
	gluLookAt(pos.x, pos.y, pos.z,
			  tx, ty, tz,
			  0.f, 1.f, 0.f);

}

void invCamera::computeVectors()
{
	static float prayon;

	prayon = cosf(avert);
	dir.y = sinf(avert);
	dir.x = sinf(ahoriz) * prayon;
	dir.z = -cosf(ahoriz) * prayon;
}

void invCamera::setSensibility(float f)
{
	sensibility = f;
}

void invCamera::begin2d()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)width, (GLdouble)height, 0.0, -100.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void invCamera::end2d()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//------------------------------------------------
// invBoard
//------------------------------------------------
invBoard::invBoard()
{
	heights = NULL;
	sizeX = sizeZ = 0.f;
	cellSize = 1.f;

	light = NULL;
}

invBoard::~invBoard()
{
	deinit();
}

void invBoard::init(int sX, int sZ, float cS)
{
	deinit();

	sizeX = sX; sizeZ = sZ;
	cellSize = cS;

	startX = -((float)sizeX/2.f) * cellSize;
	startZ = -((float)sizeZ/2.f) * cellSize;

	heights = new float*[sizeX+1];
	for(int i=0;i<sizeX+1;i++)
	{
		heights[i] = new float[sizeZ+1];
		for(int j=0;j<sizeZ+1;j++)
			heights[i][j] = 0.f;
	}

	light = new Uint8*[sizeX+1];
	for(int i=0;i<sizeX+1;i++)
	{
		light[i] = new Uint8[sizeZ+1];
		for(int j=0;j<sizeZ+1;j++)
			light[i][j] = 255;
	}
}

void invBoard::deinit()
{
	if(heights){
		for(int i=0;i<sizeX+1;i++)
			delete[] heights[i];
		delete[] heights;
	}
	heights = NULL;
	
	if(light){
		for(int i=0;i<sizeX+1;i++)
			delete[] light[i];
		delete[] light;
	}
	light = NULL;
}


//------------------------------------------------
// invTerrain
//------------------------------------------------
invTerrain::invTerrain(int sX, int sZ, float cS)
{
	texSize = 1.f;
	tex = 0;

	init(sX, sZ, cS);
}

invTerrain::~invTerrain()
{
}

void invTerrain::draw()
{
	glBindTexture(GL_TEXTURE_2D, tex);

	for(int i=0;i<sizeX;i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0.f, 1.f, 0.f);
		for(int j=0;j<sizeZ+1;j++)
		{
			glTexCoord2f((float)i * texSize, (float)j * texSize);
			glVertex3f((float)i * cellSize + startX, heights[i][j],
					   (float)j * cellSize + startZ);

			glTexCoord2f((float)(i+1) * texSize, (float)j * texSize);
			glVertex3f((float)(i+1) * cellSize + startX, heights[i+1][j],
					   (float)j * cellSize + startZ);
		}
		glEnd();
	}

}

void invTerrain::texture(GLuint texture, float size)
{
	texSize = size;
	tex = texture;
}

void invTerrain::loadHeightmap(const char* filename)
{
	SDL_Surface* hm = IMG_Load(filename);
	if(!hm)
	{
		printf("ERROR : Could not %s\n", filename);
		return;
	}

	SDL_LockSurface(hm);
	
	unsigned char* pixel = (unsigned char*)hm->pixels;
	float stepX = (float)sizeX/hm->w;
	float stepZ = (float)sizeZ/hm->h;
	for(int i=0;i<hm->w;i++){
		for(int j=0;j<hm->h;j++)
		{
			unsigned char value = pixel[i * hm->pitch + j];
			
			heights[(int)((float)i * stepX)][(int)((float)j * stepZ)] = (float)value/50.f;
		}
	}

	SDL_UnlockSurface(hm);
	SDL_FreeSurface(hm);
}

	
//------------------------------------------------
// Miscillenious Functions
//------------------------------------------------
void drawCube(float s)
{
	drawCube(s, s, s);
}
void drawCube(float x, float y, float z)
{
	glBegin(GL_QUADS);

	glVertex3f(-x, y, -z); glVertex3f(-x, y, z);
	glVertex3f(-x, -y, z); glVertex3f(-x, -y, -z);

	glVertex3f(x, y, -z); glVertex3f(x, y, z);
	glVertex3f(x, -y, z); glVertex3f(x, -y, -z);

	glVertex3f(-x, -y, -z); glVertex3f(x, -y, -z);
	glVertex3f(x, -y, z); glVertex3f(-x, -y, z);

	glVertex3f(-x, y, -z); glVertex3f(x, y, -z);
	glVertex3f(x, y, z); glVertex3f(-x, y, z);

	glVertex3f(-x, y, -z); glVertex3f(x, y, -z);
	glVertex3f(x, -y, -z); glVertex3f(-y, -y, -z);

	glVertex3f(-x, y, z); glVertex3f(x, y, z);
	glVertex3f(x, -y, z); glVertex3f(-x, -y, z);

	glEnd();
}

void drawTexturedSquare(float x, float y)
{
	glBegin(GL_QUADS);

	glTexCoord2f(0.f, 0.f); glVertex2f(-x, y);
	glTexCoord2f(1.f, 0.f); glVertex2f(x, y);
	glTexCoord2f(1.f, 1.f); glVertex2f(x, -y);
	glTexCoord2f(0.f, 1.f); glVertex2f(-x, -y);

	glEnd();
}
