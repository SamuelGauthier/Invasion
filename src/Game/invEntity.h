#ifndef INVENTITY_H
#define INVENTITY_H
typedef unsigned char Uint8;

#define ENTITY_SPEED 1.f
#define MIN_DISTANCE 0.1f
#include "invList.h"
#include "invGeom.h"
#include "invManager.h"
#include "invMath.h"
#include "pathfinding.h"
#include "invBO.h"
#include "font.h"

//----------------------------------
// invObject
//----------------------------------
class invObject {
public:
	invObject ();
	~invObject ();

	Vec3f pos;
	Vec3f rot;
	Vec3f scale;

	bool visible;
	float opacity;
	unsigned int id;
};

//------------------------------------------------
// invQuad
//------------------------------------------------
class invQuad 
{
public:
	invQuad();
	~invQuad();

	GLuint tex;
	bool in2D;
	float w, h;
};

//------------------------------------------------
// invQuadInstance
//------------------------------------------------
class invQuadInstance : public invObject
{
public:
	invQuadInstance(invQuad* q);
	~invQuadInstance();
	void draw();

	invQuad* q;
};

//----------------------------------
// invTextBuffer
//----------------------------------
class invTextBuffer : public invObject {
public:
	invTextBuffer ();
	~invTextBuffer ();

	void setFont(Font* f);
	void setText(const char* str);

	Font* font;
	char str[256];
};

//------------------------------------------------
// ModelDef
//------------------------------------------------
class ModelDef
{
public:
	char name[32];
	IMFModel* mesh;
	GLuint tex;

	float bx;
	float bz;
	int alpha;

	float scale;
	bool canMove;
	bool selectable;

	int id;
	float sight;

	ModelDef()
	{
		tex = 0;
		mesh = NULL;
		name[0] = '\0';
	}

	~ModelDef()
	{
		if(tex) glDeleteTextures(1, &tex);
		delete mesh;
	}
};

//------------------------------------------------
// invEntity
//------------------------------------------------
class invEntity : public invObject
{
public:
	invEntity(ModelDef* m);
	~invEntity();

	int state;
	
	ModelDef* mdef;
	IMFModel* mesh;
	GLuint tex;

	// keyframe animation
	float time_interval;
	int StartFrame; // starts at 0
	int EndFrame;
	int CurFrame;

	Vec3f* interpolatedDataV;
	Vec3f* interpolatedDataN;
	Vec2f* interpolatedDataT;

	// Path
	invPathNode* path;
	
	// Mass
	bool mobile;

	
	// command functions
	void orientation(float dx, float dz);
	void orientation(invEntity* e);
	void move(float elapsedTime);
	bool followPath(float elapsedTime, invPathFinderMap* map, Vec3f* dest);
	void deletePath();

	// GL functions
	void draw();

	// animation function
	void animate(float elapsedTime);
	void setMesh(IMFModel* mesh);

	char name[32];

private:
	void interpolate(float timelapse, int startframe, int nextframe);
};
#endif
