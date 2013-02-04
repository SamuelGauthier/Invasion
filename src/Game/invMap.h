#ifndef INVMAP_h
#define INVMAP_H

#include <stdio.h>
#include <stdlib.h>
#include "invTex.h"
#include "invVFS.h"

struct invMap
{
	struct CellTex
	{
		int indices[25];
		GLuint tex;

		CellTex()
		{
		}

		~CellTex()
		{
			if(glIsTexture(tex)) glDeleteTextures(1, &tex);
		}

		bool operator==(const CellTex& c) const
		{
			for(int i=0;i<25;i++)
			{
				if(c.indices[i] != indices[i])
					return false;
			}

			return true;
		}

	};

	float startx, starty;
	float cellx, celly;
	float endx, endy;
	int countx, county;

	CellTex* ct;

	invMap(int count, float size);
	~invMap();

	void loadTex(const char* map, const char* descfile);
	void draw(GLuint, bool);
};

struct invPathFinderMap
{
public:
	float startx, starty;
	float cellx, celly;
	float endx, endy;
	int countx, county;

	bool* occupied;
	bool* occupiedByMobileUnit;
	unsigned char* visible; // 0 - 256
	unsigned char* explored;

	invPathFinderMap(int count, float size);
	~invPathFinderMap();

	int getIndex(float x, float y);
	int nextTo(int i, int cell);
	void getPos(float& x, float& y, int cell);
	void setRect(float x1, float y1, float x2, float y2, bool occup = true);
	bool isAvailable(float x1, float y1, float x2, float y2);
	bool isInside(int index);
	bool isFreeCell(int index);

	int getDistance(int index1, int index2);

	bool inBound(int i, int j);
};

#endif 
