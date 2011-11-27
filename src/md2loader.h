#ifndef MD2_LOADER_H
#define MD2_LOADER_H

#define MD2_IDENT (('2'<<24) + ('P'<<16) + ('D'<<8) + 'I')
#define MD2_VERSION  8

#include "utils.h"
#include "image_utils.h"
#include "vector.h"
#include "math.h"

struct md2_header
{
	int ident;
	int version;
	int skin_width;
	int skin_height;
	int frame_size;
	
	int num_skins;
	int num_xyz;
	int num_st;
	int num_tris;
	int num_glcmds;
	int num_frames;
	
	int ofs_skins;
	int ofs_st;
	int ofs_tris;
	int ofs_frames;
	int ofs_glcmds;
	int ofs_end;
};

struct md2_st
{
	short s;
	short t;
};

struct md2_tris
{
	unsigned short vertexindex[3];
	unsigned short textureindex[3];
};

struct md2_xyz
{
	unsigned char v[3];
	unsigned char normalIndex;
};

struct md2_animations
{
	int first_frame;
	int last_frame;
	int fps;
};

struct md2_frames
{
	float scale[3];
	float translate[3];
	char name[16];
	md2_xyz* first;
};

struct GMD2Model
{
	md2_frames* frames;
	md2_tris* indices;
	md2_st* texcoords;

	int num_tris;
	int num_frames;
	int num_texcoords;

	float skinwidth;
	float skinheight;
	
	md2_animations* anim;

	float time;
	int frame;
	int next_frame;

	GLuint tex;
};

GMD2Model* loadMD2(const char* filename);
void setTexture(GMD2Model* m, const char* filename);
void render(GMD2Model* m, float ElapsedTime);
void render(GMD2Model* m, int frame);
void render(GMD2Model* m, int framea, int frameb, float inter);  // inter 0.f - 1.f
void setAnimation(GMD2Model* m, int animation_id);
char* getAnimationName(GMD2Model* m);
void releaseMD2Model(void* a);

#endif