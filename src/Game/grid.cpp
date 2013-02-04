#include "grid.h"

int absi(int x)
{
	if(x < 0)
		return -x;
	return x;
}

//----------------------------------------------
// Grid
//----------------------------------------------
Grid::Grid(int count, float size)
{
	cellx = celly = size;
	startx = starty = -((float)count/2) * size;
	endx = endy = ((float)count/2) * size;
	countx = county = count;

	occupied = new bool[countx * county];
	coeffs = new float[countx * county];

	for(int i=0;i<countx * county;i++)
	{
		occupied[i] = false;
		coeffs[i] = 0.f;
	}
}

Grid::~Grid()
{
	if(occupied) delete[] occupied;
	if(coeffs) delete[] coeffs;
}

// return -1 if the point is not in the grid
int Grid::getIndex(float x, float y)
{
	float offsetx = x - startx, offsety = y - starty;
	if(offsetx < 0.f || offsety < 0.f)
		return -1;

	int cx = (int)(offsetx/cellx), cy = (int)(offsety/celly);
	if(cx >= countx || cy >= county)
		return -1;

	return (cx * county + cy);
}

//---------------------------------------
// return -1 if there's nothing next
//
//   0    1   2
//   3    x   4
//   5    6   7
//---------------------------------------
int Grid::nextTo(int i, int cell)
{
	bool up = false, down = false, right = false, left = false;
	if(cell > county) up = true;
	if(cell % county != 0) left = true;
	if(cell % county != (county-1)) right = true;
	if(cell < (countx-1) * county) down = true;

	switch(i)
	{
		case 0: if(up && left) return (cell - 1 - county);
		case 1: if(up) return (cell - county);
		case 2: if(up && right) return (cell + 1 - county);
		case 3: if(left) return (cell - 1);
		case 4: if(right) return (cell + 1);
		case 5: if(down && left) return (cell - 1 + county);
		case 6: if(down) return (cell + county);
		case 7: if(down && right) return (cell + 1 + county);
		default: break;
	}

	return -1;
}

void Grid::getPos(float& x, float& y, int cell)
{
	int cx = cell/county;
	int cy = cell%county;

	x = startx + (float)(cx+0.5f) * cellx;
	y = starty + (float)(cy+0.5f) * celly;
}

void Grid::insertRect(float x1, float y1, float x2, float y2)
{
	int startcell = getIndex(x1, y1);
	int finishcell = getIndex(x2, y2);

	int limitx = absi((startcell/county) - (finishcell/county));
	int limity = absi((startcell%county) - (finishcell%county));

	for(int x = startcell/county;x <= startcell/county + limitx;x++)
	{
		for(int y = startcell%county;y < startcell%county + limity;y++)
		{
			occupied[x * county + y] = true;
		}
	}
}


bool Grid::isAvailable(float x1, float y1, float x2, float y2)
{
	int startcell = getIndex(x1, y1);
	int finishcell = getIndex(x2, y2);

	int limitx = absi((startcell/county) - (finishcell/county));
	int limity = absi((startcell%county) - (finishcell%county));

	for(int x = startcell/county;x <= startcell/county + limitx;x++)
	{
		for(int y = startcell%county;y < startcell%county + limity;y++)
		{
			if(occupied[x * county + y])
				return false;
		}
	}
	return true;

}
