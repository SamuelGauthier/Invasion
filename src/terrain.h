#ifndef TERRAIN_H
#define TERRAIN_H
#define F_PI 3.14159f
#include <vector>
#include <time.h>
#include "utils.h"
#include "vector.h"
#include "math.h"

enum TERRAIN_SIZE
{
	TERRAIN_SIZE_64x64 = 64
};

struct GTerrain
{
	GLuint VBO;	
	GLuint IBO;
	int numIndices;

	int width;
	float cell;
	float mntsize;

	GLuint texture[2];
	float tex_size;
};

float* loadHeightMap(const char* filename, int& size);
// GTerrain* generateTerrain(const int width, const int max, const int min);
// void createMountain(float* heights,const  int width, const int x, const int y, const float height, const float radius);
Vec3f* terrainNormals(float* map, int size);
void setTexture(GTerrain* t, const char* filename, int stage);
void render(const GTerrain* t);
void releaseTerrain(void* t);
void fillBuffers(GTerrain* trn, float* height);

#endif
