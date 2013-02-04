#include "invTree.h"

//------------------------------------------------
// invTree
//------------------------------------------------
inline invTree::invTree(float _x, float _y, float _size)
{
	set(_x, _y, _size);
	pChild = NULL;
	state = FREE;
}

invTree::~invTree()
{
	shrink();
}

void invTree::set(float _x, float _y, float _size)
{
	x = _x; y = _y;
	size = _size;
}

//------------------------------------------------
// (0,0) +------+------+ (1,0)
//       |	1   |  3   |
//	     |	    |      |
//       +------+------+
//       |	2   |  4   |
//       |      |      |
// (0,1) +------+------+ (1,1)
//------------------------------------------------

void invTree::expand()
{
	if(!pChild)
	{
		pChild = new invTree[4];
		pChild[0].set(x, y, size/2.f);
		pChild[1].set(x, y + size/2.f, size/2.f);
		pChild[2].set(x + size/2.f, y, size/2.f);
		pChild[3].set(x + size/2.f, y + size/2.f, size/2.f);
	}
}

void invTree::shrink()
{
	if(pChild) { // only 0 or 4 pChild can be allocated
		delete[] pChild;
		pChild = NULL;
	}
}

// TODO Optimize this
inline bool invTree::inRangeOf(float x1, float y1, float x2, float y2)
{
	printf("in\n");
	bool result = (x1 < x + size && x2 > x && y1 < y + size && y2 > y);
	printf("result found\n");
	return result;
}

inline bool invTree::inRangeOf(float x1, float y1)
{
	return (x1 >= x && x1 < x + size && y1 >= y && y1 < y + size);
}

void invTree::draw()
{
	if(pChild) {
		for(int i=0;i<4;i++)
			pChild[i].draw();
	}

	else
	{
		if(state == FREE)
		{
			glColor3f(1.f, 1.f, 0.f);
			glVertex3f(x, 0.1f, y);
			glVertex3f(x+size, 0.1f, y);
			glVertex3f(x+size, 0.1f, y+size);
			glVertex3f(x, 0.1f, y+size);
		}
		if(state == OCCUPIED)
		{
			glColor3f(1.f, 0.f, 0.f);
			glVertex3f(x, 0.1001f, y);
			glVertex3f(x+size, 0.1001f, y);
			glVertex3f(x+size, 0.1001f, y+size);
			glVertex3f(x, 0.1001f, y+size);
		}
	}
}

void invTree::updateState()
{
	if(pChild)
	{
		int index = 0;
		for(int i=0;i<4;i++)
		{
			if(pChild[i].state == OCCUPIED)
				index += 2;
			if(pChild[i].state == PARTIALLY_OCCUPIED)
				index += 1;
		}
		if(index == 0)
			state = FREE;
		else if(index == 8)
			state = OCCUPIED;
		else
			state = PARTIALLY_OCCUPIED;
	}
}

//------------------------------------------------
// Toolbox for invTree class
//------------------------------------------------
invTree* generateTree(float x, float y, float size, int num)
{
	float cellsize = size/(float)num;
	invTree* t = new invTree[num * num];
	for(int i=0;i<num;i++)
	{
		for(int j=0;j<num;j++)
		{
			t[i * num + j].x = (float)i * cellsize + x;
			t[i * num + j].y = (float)j * cellsize + y;
			t[i * num + j].size = cellsize;
		}
	}

	return t;
}

void drawTrees(invTree* t, int num)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);
	for(int i=0;i<num*num;i++)
		t[i].draw();
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void insertRect(invTree* t, int num, float x1, float y1, float x2, float y2, int limit)
{
	if(limit == 3)
	{
		for(int i=0;i<num*num;i++)
		{
			if(t[i].inRangeOf(x1, y1, x2, y2))
			{
				t[i].state = OCCUPIED;
			}
			else
			{
				t[i].state = FREE;
			}
		}
	}

	else
	{
		invTree* tree = NULL;
		float deltaX = (t[num*num-1].x+t[num*num-1].size) - t[0].x;
		float deltaY = (t[num*num-1].y+t[num*num-1].size) - t[0].y;

		float sizeX = deltaX/(float)num;
		float sizeY = deltaY/(float)num;

		int i1 = (int)((x1 - t[0].x)/sizeX);
		int j1 = (int)((y1 - t[0].y)/sizeY);
		int i2 = (int)((x2 - t[0].x)/sizeX);
		int j2 = (int)((y2 - t[0].y)/sizeY);

		for(int i=i1;i<=i2;i++)
		{
			if(i < 0 || i >= num)
				continue;

			for(int j=j1;j<=j2;j++)
			{
				if(j < 0 || j >= num)
					continue;

				printf("begin\n");
				tree = &t[i * num + j];

				if(tree->inRangeOf(x1, y1, x2, y2))
				{
					tree->expand();
					printf("limit : %d\n", limit+1);
					insertRect(tree->pChild, 2, x1, y1, x2, y2, limit+1);
					tree->updateState();
				}
				else
					printf("end2\n");
			}
		}
	}
}
