#include "invMap.h"

//----------------------------------------------
// invMap
//----------------------------------------------
invMap::invMap(int count, float size)
{
	cellx = celly = size;
	startx = starty = -((float)count/2) * size;
	endx = endy = ((float)count/2) * size;
	countx = county = count;

	ct = new CellTex[countx * county];
}

invMap::~invMap()
{
	if(ct) delete[] ct;
}

void invMap::draw(GLuint visible, bool tex)
{
	if(tex)
	{
		glActiveTextureARB(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, visible);

		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
	}

	GLuint curTex = 0;
	for(int j=0;j<county;j++)
	{
		for(int i=0;i<countx;i++)
		{
			if(curTex != ct[j * county + i].tex && tex)
			{
				curTex = ct[j * county + i].tex;
				glBindTexture(GL_TEXTURE_2D, curTex);
			}

			glBegin(GL_TRIANGLE_STRIP);
			glNormal3f(0.f, 1.f, 0.f);

			if(tex)
			{
				glMultiTexCoord2f(GL_TEXTURE0, 1.f, 0.f);
				glMultiTexCoord2f(GL_TEXTURE1, (float)(i+1)/(float)countx,
											   (float)(j)/(float)county);
			}

			glVertex3f((float)(i+1) * cellx + startx, 0.f,
					   (float)j * celly + starty);

			if(tex)
			{
				glMultiTexCoord2f(GL_TEXTURE0, 0.f, 0.f);
				glMultiTexCoord2f(GL_TEXTURE1, (float)(i)/(float)countx,
											   (float)(j)/(float)county);
			}

			glVertex3f((float)i * cellx + startx, 0.f,
					   (float)j * celly + starty);

			if(tex)
			{
				glMultiTexCoord2f(GL_TEXTURE0, 1.f, 1.f);
				glMultiTexCoord2f(GL_TEXTURE1, (float)(i+1)/(float)countx,
											   (float)(j+1)/(float)county);
			}

			glVertex3f((float)(i+1) * cellx + startx, 0.f,
					   (float)(j+1) * celly + starty);

			if(tex)
			{
				glMultiTexCoord2f(GL_TEXTURE0, 0.f, 1.f);
				glMultiTexCoord2f(GL_TEXTURE1, (float)(i)/(float)countx,
											   (float)(j+1)/(float)county);
			}

			glVertex3f((float)i * cellx + startx, 0.f,
					   (float)(j+1) * celly + starty);
			glEnd();
		}
	}

	if(tex)
	{
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE0);
	}
}

void invMap::loadTex(const char* mapfile, const char* descfile)
{
	SDL_Surface* map = loadImage(s_vfs->texture(mapfile));
	if((map->w-1) % 4 || (map->h-1) % 4)
	{
		printf("ERROR : %s has incorrect dimensions\n", mapfile);
		return;
	}


	SDL_Surface* surf[16];
	int nb_surf = 0;

	FILE* desc = fopen(descfile, "r");
	if(!desc)
	{
		printf("ERROR : Cannot open %s\n", descfile);
		return;
	}

	char line[128];
	while(fgets(line, 128, desc))
	{
		char* ptr = line;
		while(*ptr)
		{
			if(*ptr < 20)
			{
				*ptr = '\0';
				break;
			}
			ptr++;
		}

		surf[nb_surf] = loadImage(s_vfs->texture(line));
		SDL_LockSurface(surf[nb_surf++]);
	}

	fclose(desc);

	SDL_LockSurface(map);

	Uint8* m = (Uint8*)map->pixels;

	for(int j=0;j<county;j++)
	{
		for(int i=0;i<countx;i++)
		{
			// get types
			int index = j * county + i;
			for(int k=0;k<5;k++)
			{
				for(int l=0;l<5;l++)
				{
					ct[index].indices[k*5+l]=(m[(j*4+k)*map->pitch + i*4+l])/20;
				}
			}

			// search if not already generated
			bool found = false;
			for(int k=0;k<index;k++)
			{
				if(ct[index] == ct[k])
				{
					ct[index].tex = ct[k].tex;
					found = true;
					break;
				}
			}

			if(found) continue;

			// generate it
			bool allEqual = true;
			for(int i=1;i<25;i++)
			{
				if(ct[index].indices[0] != ct[index].indices[i])
				{
					allEqual = false;
					break;
				}
			}

			if(allEqual)
			{
				ct[index].tex = cvtSDLSurfToGLTex(surf[ct[index].indices[0]]);
			}
			else
			{
				SDL_Surface* splat = interpolate(ct[index].indices, 4, &surf[0]);
				ct[index].tex = cvtSDLSurfToGLTex(splat);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				SDL_FreeSurface(splat);
			}
		}
	}

	SDL_UnlockSurface(map);

	for(int i=0;i<nb_surf;i++)
	{
		SDL_UnlockSurface(surf[i]);
		SDL_FreeSurface(surf[i]);
	}

	SDL_FreeSurface(map);
}

//------------------------------------------------
// invPathFinderMap
//------------------------------------------------
invPathFinderMap::invPathFinderMap(int count, float size)
{
	cellx = celly = size;
	startx = starty = -((float)count/2) * size;
	endx = endy = ((float)count/2) * size;
	countx = county = count;

	occupied = new bool[countx * county];
	occupiedByMobileUnit = new bool[countx * county];
	visible = new unsigned char[countx * county];
	explored = new unsigned char[countx * county];

	for(int i=0;i<countx * county;i++)
	{
		occupied[i] = false;
		occupiedByMobileUnit[i] = false;
		visible[i] = 0;
		explored[i] = 0;
	}
}

invPathFinderMap::~invPathFinderMap()
{
	if(occupied) delete[] occupied;
	if(occupiedByMobileUnit) delete[] occupiedByMobileUnit;
}

// return -1 if the point is not in the invPathFinderMap
int invPathFinderMap::getIndex(float x, float y)
{
	float offsetx = x - startx, offsety = y - starty;
	if(offsetx < 0.f || offsety < 0.f)
		return -1;

	int cx = (int)(offsetx/cellx), cy = (int)(offsety/celly);
	if(cx >= countx || cy >= county)
		return -1;

	return (cy * countx + cx);
}

//---------------------------------------
// return -1 if there's nothing next
//
//   0    1   2
//   3    x   4
//   5    6   7
//---------------------------------------
int invPathFinderMap::nextTo(int i, int cell)
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

void invPathFinderMap::getPos(float& x, float& y, int cell)
{
	int cx = cell%county;
	int cy = cell/county;

	x = startx + (float)(cx+0.5f) * cellx;
	y = starty + (float)(cy+0.5f) * celly;
}

void invPathFinderMap::setRect(float x1, float y1, float x2, float y2, bool occup)
{
	int startcell = getIndex(x1, y1);
	int finishcell = getIndex(x2, y2);

	int limitx = abs((startcell/county) - (finishcell/county));
	int limity = abs((startcell%county) - (finishcell%county));

	for(int x = startcell/county;x <= startcell/county + limitx;x++)
	{
		for(int y = startcell%county;y <= startcell%county + limity;y++)
		{
			occupied[x * county + y] = occup;
		}
	}
}


bool invPathFinderMap::isAvailable(float x1, float y1, float x2, float y2)
{
	int startcell = getIndex(x1, y1);
	int finishcell = getIndex(x2, y2);

	int limitx = abs((startcell/county) - (finishcell/county));
	int limity = abs((startcell%county) - (finishcell%county));

	for(int x = startcell/county;x <= startcell/county + limitx;x++)
	{
		for(int y = startcell%county;y < startcell%county + limity;y++)
		{
			if(occupied[x * county + y] || occupiedByMobileUnit[x * county + y])
				return false;
		}
	}
	return true;
}

bool invPathFinderMap::isInside(int index)
{
	return (index >= 0) && (index < countx * countx);
}

int invPathFinderMap::getDistance(int index1, int index2)
{
	int dx = (index1/county) - (index2/county);
	int dy = (index1%county) - (index2%county);

	return (dx * dx + dy * dy);
}

bool invPathFinderMap::isFreeCell(int index)
{
	return (!occupied[index] && !occupiedByMobileUnit[index]);
}

bool invPathFinderMap::inBound(int i, int j)
{
	return (i > 0 && i < countx && j > 0 && j < county);
}

