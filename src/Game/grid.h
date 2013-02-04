#ifndef GRID_H
#define GRID_H

struct Grid
{
	float startx, starty;
	float cellx, celly;
	float endx, endy;
	int countx, county;

	bool* occupied;
	float* coeffs;

	Grid(int count, float size);
	~Grid();

	int getIndex(float x, float y);
	int nextTo(int i, int cell);
	void getPos(float& x, float& y, int cell);
	void insertRect(float x1, float y1, float x2, float y2);
	bool isAvailable(float x1, float y1, float x2, float y2);
};

int absi(int x);

#endif 
