#include "invGrid.h"

//------------------------------------------------
// invArea
//------------------------------------------------
invArea::invArea(float _x, float _y, float _size) : x(_x), y(_y), size(_size), occupied(false)
{
}

bool invArea::inRangeOf(float x1, float y1, float x2, float y2) const
{
	return (x1 < x + size && x2 > x && y1 < y + size && y2 > y);
}

bool invArea::inRangeOf(float x1, float y1) const
{
	return (x1 >= x && x1 < x + size && y1 >= y && y1 < y + size);
}

Vec3f invArea::getPos() const
{
	return Vec3f(x + size/2.f, 0.f, y + size/2.f);
}

//------------------------------------------------
// invNode
//------------------------------------------------
invNode::invNode(invArea* a, invNode* p)
{
	parent = p;
	area = a;
}
	
invNode::invNode(invArea* a, Vec3f* finish, int sp, invNode* p)
{
    step = sp;
    area = a;
    parent = p;

    setHeuristic(finish);
}

void invNode::setHeuristic(Vec3f* finish)
{
	int deltaX = (int)((finish->x - area->x) * 50.f);
	int deltaY = (int)((finish->z - area->y) * 50.f);
	heuristic = deltaX * deltaX + deltaY * deltaY;
}

void deletePath(invNode* path)
{
	while(path)
	{
		invNode* tmp = path;
		path = path->parent;
		delete tmp;
	}
}
//------------------------------------------------
// invGrid
//------------------------------------------------
invGrid::invGrid(float startx, float startz, float size, int num)
{
	cellsize = size/(float)num;
	x = startx;
	y = startz;
	count = num;

	invArea* area = NULL;
	a = new invArea[count* count];
	for(int i=0;i<count;i++)
	{
		for(int j=0;j<count;j++)
		{
			area = &a[i * count + j];

			area->x = (float)i * cellsize + x;
			area->y = (float)j * cellsize + y;
			area->size = cellsize;

			for(int k=0;k<8;k++)
				area->nextTo[k] = 0;

			int index = 0;
			if(i > 0)
				area->nextTo[index++] = &a[(i-1) * count + j];
			if(j > 0)
				area->nextTo[index++] = &a[i * count + j-1];
			if(i < count-1)
				area->nextTo[index++] = &a[(i+1) * count + j];
			if(j < count-1)
				area->nextTo[index++] = &a[i * count + j+1];
			if(i > 0 && j > 0)
				area->nextTo[index++] = &a[(i-1) * count + j-1];
			if(i > 0 && j < count-1)
				area->nextTo[index++] = &a[(i-1) * count + j+1];
			if(i < count-1 && j < count-1)
				area->nextTo[index++] = &a[(i+1) * count + j+1];
			if(i < count-1 && j > 0)
				area->nextTo[index++] = &a[(i+1) * count + j-1];

		}
	}
}

invGrid::~invGrid()
{
	if(a)
		delete[] a;
}

// TODO : optimize this
void invGrid::insertRect(float x1, float y1, float x2, float y2)
{
	invArea* area = 0;
	int i1 = (int)((x1 - a[0].x)/a[0].size);
	int j1 = (int)((y1 - a[0].y)/a[0].size);
	int i2 = (int)((x2 - a[0].x)/a[0].size);
	int j2 = (int)((y2 - a[0].y)/a[0].size);

	for(int i=i1;i<=i2;i++)
	{
		if(i < 0 || i >= count)
			continue;

		for(int j=j1;j<=j2;j++)
		{
			if(j < 0 || j >= count)
				continue;

			area = &a[i * count + j];
			if(area->inRangeOf(x1, y1, x2, y2))
				area->occupied = true;
		}
	}
}

invArea* invGrid::findArea(Vec3f* pos)
{
	int i = (int)((pos->x - a[0].x)/a[0].size);
	int j = (int)((pos->z - a[0].y)/a[0].size);

	if(i >= 0 && i < count && j >= 0 && j < count)
		return &a[i * count + j];
	return 0;
}

int invGrid::getIndex(Vec3f* pos)
{
	int i = (int)((pos->x - a[0].x)/a[0].size);
	int j = (int)((pos->z - a[0].y)/a[0].size);

	if(i >= 0 && i < count && j >= 0 && j < count)
		return (i * count + j);
	return -1;
}

invNode* invGrid::findPath(Vec3f* start, Vec3f* finish)
{
    invNode f(findArea(finish), finish);
    Vec3f new_finish = Vec3f(f.area->x, 0.f, f.area->y);
	invNode *s = new invNode(findArea(start), &new_finish);

	if(!s->area || !f.area)
		return NULL;
	invNode* next = NULL;
	invArea* nextTo = NULL;

	invList<invNode> openList;
	invList<invNode>* closedList = new invList<invNode>;

	openList.add(s);
	while(!openList.isEmpty())
	{
        openList.rewind();
		invNode* current = openList.current();
		int min = current->heuristic;
		while(!openList.isEnd())
		{
			if(min > openList.current()->heuristic){
				min = openList.current()->heuristic;
				current = openList.current();
			}
			openList.next();
		}

        openList.seek(current);
        openList.remove();
		closedList->add(current);

		openList.rewind();

		if(current->area == f.area) break;

		for(int i=0;i<8 && current->area->nextTo[i];i++)
		{
			nextTo = current->area->nextTo[i];
			if(nextTo->occupied) continue;

			openList.rewind();

			bool nextAdj = false;
			while(!openList.isEnd())
			{
                if(openList.current()->area == nextTo)
                {
                    if(openList.current()->step > current->step + 1)
                    {
                        openList.current()->parent = current;
                        openList.current()->step = current->step + 1;
                        nextAdj = true;
                        break;
                    }
                }
                openList.next();
			}
			if(nextAdj) continue;

			closedList->rewind();
			while(!closedList->isEnd()) {
				if(closedList->current()->area == nextTo) {
				    nextAdj = true;
				    break;
				}
				closedList->next();
			}
			if(nextAdj) continue;

            next = new invNode(nextTo, &new_finish, current->step + 1, current);
			openList.add(next);
		}
	}

	invNode* final_path = recreatePath(closedList);
	return final_path;
}

invNode* invGrid::recreatePath(invList<invNode>* closedList)
{
	invNode* n = NULL;
	invNode* path = closedList->getHead();
	invNode* prev = NULL;
	
	while(path)
	{
		n = new invNode(path->area, prev);
		prev = n;

		path = path->parent;
	}

	return n;
}
