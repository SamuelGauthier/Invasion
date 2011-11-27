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

	GLuint texture[3];
	float tex_size;
	int tex_num;

	float* height;
};

GTerrain* initTerrain();
void loadHeightMap(GTerrain* t, const char* filename);
void randomGen(GTerrain* t, int freq, int height, int chance);
Vec3f* terrainNormals(float* map, int size);
void setTexture(GTerrain* t, const char* filename, int stage);
void render(const GTerrain* t);
void releaseTerrain(void* t);
void fillBuffers(GTerrain* trn);
void planeTerrain(GTerrain* t, int width);

#endif
