#ifndef INVGEOM_H
#define INVGEOM_H
#define MD2_IDENT (('2'<<24) + ('P'<<16) + ('D'<<8) + 'I')
#define MD2_VERSION  8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <SDL/SDL_image.h>
#include <assert.h>

namespace Terrain
{
	extern int width,length;
	extern float** data;
	extern float size;
	extern GLuint tex;

	void init();
	void render();
	GLuint generateLightmap();
	void destroy(void*);
} /* Terrain */

namespace IMF
{
	struct Triangle
	{
		Vec3f v[3];
		Vec3f n[3];
		Vec2f t[3];
	};

	struct Model
	{
		unsigned int numTri;
		Triangle* t;
		GLuint tex;
	};
	
	Model* load(const char* filename);
	void render(void* m, float);
	void deinit(void* tmp);
} /* OBJ */

namespace MD2 
{
	struct Header
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

	struct St
	{
		short s,t;
	};

	struct Tri
	{
		unsigned short vertexindex[3];
		unsigned short textureindex[3];
	};

	struct XYZ
	{
		unsigned char v[3];
		unsigned char normalIndex;
	};

	struct Frame
	{
		float scale[3];
		float translate[3];
		char name[16];
		XYZ* first;
	};

	struct Animation
	{
		int first_frame;
		int last_frame;
		int fps;
	};

	struct Model
	{
		GLuint tex;	
		
		int num_tris;		
		int num_st;
		int num_frames;

		Frame* frames;
		Tri* tris;
		St* texcoords;
		
		Animation* anim;
		float time;
		int frame;
		int next_frame;

		float skinwidth, skinheight;
	};

	Model* load(const char* filename);
	void render(MD2::Model* m, int framea, int frameb, float inter);
	void render(void* m, float ElapsedTime);
	void setAnimation(void* m, int);
	void deinit(void*);
} /* MD2 */
#endif
