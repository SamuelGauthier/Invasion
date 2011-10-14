#ifndef BUFFER_H
#define BUFFER_H

#include "utils.h"
#include "vector.h"

enum
{
	BUFFER_16KB = 16 * 1024,
	BUFFER_32KB = 2 * BUFFER_16KB,
	BUFFER_64KB = 2 * BUFFER_32KB,
	BUFFER_128KB = 2 * BUFFER_64KB,
	BUFFER_256KB = 2 * BUFFER_128KB,
	BUFFER_512KB = 2 * BUFFER_256KB,
	BUFFER_1MB = 2 * BUFFER_512KB,
	BUFFER_2MB = 2 * BUFFER_1MB,
	BUFFER_4MB = 2 * BUFFER_2MB
};

// Bon pour les objets répétitifs comme les arbres 
struct GStaticBuffer
{
	GLuint VBO;
	GLuint IBO;

	int start[32];
	int num;

	GLuint tex[16];
	int mode[16];

	int curstart;
	int curcount;
	int curend;
};

GStaticBuffer* initStaticBuffer(int sizeVBO, int sizeIBO);
int addStaticBuffer(GStaticBuffer* sb, Vec3f* vertices, Vec3f* normals, Vec2f* texcoords, int numVert,  ushort* indices, int numInd, int mode);
int addStaticBuffer(GStaticBuffer* sb, float* VertexData, int numVert, ushort* indices, int numInd, int mode);
void setTexture(GStaticBuffer* sb, const char* filename, int index);
void setTexture(GStaticBuffer* sb, GLuint texture, int index);
void prepareObject(GStaticBuffer* sb, int index);
void beginRender(GStaticBuffer* sb);
void endRender();
void render(GStaticBuffer* sb, int index);
void releaseStaticBuffer(void* sb);

#endif
