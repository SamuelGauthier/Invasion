#ifndef INVGRID_H
#define INVGRID_H

#include "invMath.h"
#include "invList.h"
#include "invVFS.h"

class invArea
{
public:
	invArea(float x=0.f, float y=0.f, float size=0.f);
	float x,y;
	float size;
	invArea* nextTo[8];

	bool inRangeOf(float x1, float y1, float x2, float y2) const;
	bool inRangeOf(float x1, float y1) const;
	bool occupied;
	Vec3f getPos() const;
};

class invNode
{
public:
	invNode(invArea* a, invNode* parent);
    invNode(invArea* a, Vec3f* finish, int sp = 0, invNode* parent = NULL);
    int heuristic;
	int step;

	invNode* parent;
	invArea* area;

    void setHeuristic(Vec3f* finish);
};

void deletePath(invNode* path);

class invGrid
{
public:
	invGrid(float startx=0.f, float startz=0.f, float size=0.f, int num=0);
	~invGrid();
	invArea* a;
	float x, y;
	float cellsize;
	int count;

	void insertRect(float x1, float y1, float x2, float y2);
	invNode* findPath(Vec3f* start, Vec3f* finish);
	invArea* findArea(Vec3f* pos);
	invNode* recreatePath(invList<invNode>* closedList);
	int getIndex(Vec3f* pos);
};


#endif
