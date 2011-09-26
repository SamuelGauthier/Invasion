#ifndef MESH_H
#define MESH_H
#define F_PI 3.14159f
#include <GL/glu.h>
#include "utils.h"
#include "image_utils.h"
#include "vector.h"
#include "math.h"


struct GTerrain
{
	float* height;
	int width, length;
	float cell;

	GLuint texture;
	float tex_size;
};

GTerrain* loadHeightMap(const char* filename, const float ScaleY = 1.f, const float ScaleXZ = 1.f);
GTerrain* generateTerrain(const uint width, const uint max, const uint min);
void createMountain(float* heights,const  uint width, const int x, const int y, const float height, const float radius);
void setTexture(GTerrain* t, const char* filename, const float shrink = 1.f);
void render(const GTerrain* t);
void releaseTerrain(void* t);

struct GOBJModel
{
	Vec3f *vertices3, *vertices4;
	Vec3f *normals3, *normals4;
	Vec2f *texcoords3, *texcoords4;

	uint num3, num4;
	GLuint texture;
};

GOBJModel* loadOBJ(const char* filename);
void setTexture(GOBJModel* m, const char* filename);
void render(const GOBJModel* m);
void releaseOBJ(void* m);

#endif
