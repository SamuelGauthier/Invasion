#ifndef MESH_H
#define MESH_H
#define F_PI 3.14159f
#include <GL/glu.h>
#include "utils.h"
#include "image_utils.h"
#include "vector.h"
#include "math.h"
#include "buffer.h"


struct GOBJModel
{
	Vec3f *vertices3, *vertices4;
	Vec3f *normals3, *normals4;
	Vec2f *texcoords3, *texcoords4;

	int num3, num4;
	GLuint texture;
};

GOBJModel* loadOBJ(const char* filename);
void setTexture(GOBJModel* m, const char* filename);
void render(const GOBJModel* m);
void releaseOBJ(void* m);
int fillBuffer(GStaticBuffer* sb, GOBJModel* m);

#endif
