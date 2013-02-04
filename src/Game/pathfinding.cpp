#include "pathfinding.h"
#include <stdio.h>
#include <stdlib.h>
#define PEN_NODE 100.f
#define MAX_SEARCH_RADIUS 10

//------------------------------------------
// invPathNode
//------------------------------------------
invPathNode::invPathNode()
{
	x = y = 0.f;
	pNext = (invPathNode*)0;
	index = -1;
}

invPathNode::~invPathNode()
{
}

invPathNode* searchPath(float sx, float sy, float fx, float fy, invPathFinderMap* map)
{
	invPathNode* openList = (invPathNode*)0, *closeList = (invPathNode*)0;

	// search the first node
	int startcell = map->getIndex(sx, sy);
	int finishcell = map->getIndex(fx, fy);
	if(!map->isFreeCell(finishcell)){
		finishcell = searchBestNear(startcell , finishcell, map);
		map->getPos(fx, fy, finishcell);
	}

	if(startcell == -1 || finishcell == -1 || !map->isFreeCell(finishcell)) return (invPathNode*)0;


	// create the first node
	float posx, posy;
	invPathNode* n = new invPathNode;
	map->getPos(posx, posy, startcell);
	n->x = posx; n->y = posy;
	n->index = startcell;
	n->pPrevious = NULL;
	n->pNext = (invPathNode*)0;

	// put it in the open list
	openList = n;
	
	while(openList)
	{
		// search the best one
		invPathNode* openListBestIt = openList;
		invPathNode* openListBestPrev = (invPathNode*)0;
		invPathNode* openListBest = (invPathNode*)0;
		invPathNode* openListPrev = (invPathNode*)0;
		float bestHist = 0.f, hist, dx, dy;
		while(openListBestIt)
		{
			dx = fx - openListBestIt->x;
			dy = fy - openListBestIt->y;
			hist = dx*dx + dy*dy;//  + map->coeffs[openListBestIt->index];
			openListBestIt->hist = hist;
			if((bestHist > hist) || !openListBest)
			{
				bestHist = hist;
				openListBest = openListBestIt;
				openListBestPrev = openListPrev;
			}

			openListPrev = openListBestIt;
			openListBestIt = openListBestIt->pNext;
		}


		// put the best one in the close list and remove it from the open list
		if(openListBestPrev) openListBestPrev->pNext = openListBest->pNext;
		else openList = openList->pNext;

		openListBest->pNext = closeList;
		closeList = openListBest;

		// if the best node is the finish node, quit
		if(openListBest->index == finishcell) break;

		// put all the neighbours in the open list
		for(int i=0;i<8;i++)
		{
			int index = map->nextTo(i, openListBest->index);
			if(!map->isFreeCell(index) || index == -1) continue;

			// not already in close list
			invPathNode* closeListIt = closeList;
			bool found = false;
			while(closeListIt){
				if(closeListIt->index == index){
					found = true;
					break;
				}
				closeListIt = closeListIt->pNext;
			}
			if(found) continue;

			// not already in open list
			invPathNode* openListIt = openList;
			while(openListIt)
			{
				if(openListIt->index == index)
				{
					found = true;
					break;
				}
				openListIt = openListIt->pNext;
			}
			if(found) continue;

			map->getPos(posx, posy, index);

			invPathNode* pn = new invPathNode;
			pn->x = posx; pn->y = posy;
			pn->index = index;

			pn->pNext = openList;
			pn->pPrevious = openListBest;
			openList = pn;
		}
	}
	
	// put the actual position in the finish node
	closeList->x = fx; 
	closeList->y= fy;

	// TODO: fix the memory leaks
	// reverse the close list
	invPathNode* finalPath = closeList;
	finalPath->pNext = (invPathNode*)0;
	while(finalPath->pPrevious)
	{
		finalPath->pPrevious->pNext = finalPath;
		finalPath = finalPath->pPrevious;
	}

	// if more than one remove the first one
	if(finalPath->pNext) finalPath = finalPath->pNext;

	return finalPath;
}

int searchBestNear(int start, int finish, invPathFinderMap* map)
{
	int index = finish - MAX_SEARCH_RADIUS * map->county - MAX_SEARCH_RADIUS, cur;
	int best = -1;
	int best_distance = 0;

	for(int i=0;i<MAX_SEARCH_RADIUS*2;i++)
	{
		for(int j=0;j<MAX_SEARCH_RADIUS*2;j++)
		{
			cur = index + i * map->county + j;
			if((map->isFreeCell(cur) || cur == start) && map->isInside(cur))
			{
				int distance = map->getDistance(cur, finish);
				if(distance < best_distance || best == -1)
				{
					best = cur;
					best_distance = distance;
				}

				else if(distance == best_distance)
				{
					if(map->getDistance(cur, start) < map->getDistance(best, start))
					{
						best = cur;
						best_distance = distance;
					}
				}
			}
		}
	}

	return best;
}
