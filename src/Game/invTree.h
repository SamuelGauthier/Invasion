#ifndef INVTREE_H
#define INVTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>

enum TREE_STATE
{
	FREE = 1,
	PARTIALLY_OCCUPIED,
	OCCUPIED,
};

class invTree
{
public:
	invTree(float _x=0.f, float _y=0.f, float _size=0.f);
	~invTree();

	void expand();
	void shrink();
	bool inRangeOf(float x1, float y1, float x2, float y2);
	bool inRangeOf(float x1, float y1);
	void draw();
	void updateState();
	void set(float _x, float _y, float _size);
	invTree* pChild;

	float size;
	float x,y;
	int state;
};

invTree* generateTree(float x, float y, float size, int num);
void drawTrees(invTree* t, int num);
void insertRect(invTree* t, int num, float x1, float y1, float x2, float y2, int limit=0);

#endif
