/*
TODO : RTS Camera
*/
#ifndef INVSCENE_H
#define INVSCENE_H
#include "invMath.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glew.h>

enum CAMERA_MODE
{
	FIXED_CAM = 1,
	FPS_CAM,
	RTS_CAM,
};

class invCamera
{
public:
	Vec3f pos;
	Vec3f dir;
	Vec3f speed;
	float ahoriz, avert;
	CAMERA_MODE mode;
	int width, height;
	float sensibility;

	invCamera(CAMERA_MODE m = FIXED_CAM);
	~invCamera();

	void setSensibility(float f);
	void setPos(Vec3f p);
	void setAngles(float h, float v);
	void setHeight(float height);
	void init(int width, int height , float fov = 45.f, float fmin = 1.f, float fmax = 500.f);
	void control(float elapsedTime);
	void look();
	void lookAt(float tx, float ty, float tz);
	void reset();

	void begin2d();
	void end2d();

private:
	void computeVectors();
};

class invBoard
{
public:
	invBoard();
	~invBoard();

	float** heights;
	int sizeX, sizeZ;
	float cellSize;
	float startX, startZ;

	Uint8** light;

	void init(int sX=10, int sZ=10, float cS=1.f);
	void deinit();
};


class invTerrain : public invBoard
{
public:
	invTerrain(int sizeX = 10, int sizeZ = 10, float cellSize = 1.f);
	~invTerrain();
	void draw();
	void texture(GLuint tex, float size = 1.f);
	void loadHeightmap(const char* filename);

	float texSize;
	GLuint tex;
};

void drawCube(float s);
void drawCube(float x, float y, float z);
void drawTexturedSquare(float x, float y);

#endif
