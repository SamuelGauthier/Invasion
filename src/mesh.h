#ifndef MESH_H
#define MESH_H
#include <GL/glu.h>
#include "utils.h"
#include "image_utils.h"
#include "vector.h"


class GTerrain
{
public:
	GTerrain(const char* filename, float fScaleY = 1.f, float fScaleXZ = 1.f);

	void OnLoadTexture(const char* filename);
	void OnLoad(const char* filename, float fScaleY = 1.f, float fScaleXZ = 1.f);
	void OnRender() const; 
	void OnSubdivide();

	~GTerrain();

	float* height;
	int width, length;
	float cellX, cellZ;

	GLuint texture;
	float tex_size;
};

class GOBJModel 
{
public:
	GOBJModel();
	GOBJModel(const char* filename);
	~GOBJModel();

	
	void OnLoad(const char* filename);
	void OnRender() const;

	Vec3f *vertices3, *vertices4;
	Vec3f *normals3, *normals4;
	Vec2f *texcoords3, *texcoords4;

	uint num3, num4;
	GLuint texture;
};

GLuint loadTexture(const char* filename);

#endif
