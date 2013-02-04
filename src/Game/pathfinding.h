#ifndef PATHFINDING_H
#define PATHFINDING_H
#include "invMap.h"
#include "invMath.h"

struct invPathNode
{
	invPathNode* pNext;
	invPathNode* pPrevious;
	float x, y;
	int index;

	float hist;

	invPathNode();
	~invPathNode();
};

invPathNode* searchPath(float sx, float sy, float fx, float fy,
				invPathFinderMap* m);
int searchBestNear(int start, int finish, invPathFinderMap* map);
#endif
