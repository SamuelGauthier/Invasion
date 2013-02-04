#ifndef INVGEOM_H
#define INVGEOM_H

/*
	TODO
	
		- Remplacer les instructions de dessin immédiats
		  par des appels indirects avec l'aide des buffers
*/

#include "invMath.h"
#include "invTex.h"
#include "invScene.h"
#include "invBO.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#define OFFSET(x,y) (void*)((char*)x + y)

struct Triangle
{
	Vec3f v0;
	Vec3f n0;
	Vec2f t0;

	Vec3f v1;
	Vec3f n1;
	Vec2f t1;

	Vec3f v2;
	Vec3f n2;
	Vec2f t2;
};

class IMFModel 
{
public:
	int numTris;
	Triangle* tris;
	char name[32];
	int numFrames;
	invBufferObject bo;

	IMFModel();
	~IMFModel();

	void load(FILE* f);
	void unload();

	bool isStatic;
};

#endif
