#include "terrain.h"

GTerrain* initTerrain()
{
	GTerrain* t = new GTerrain;

	t->width = 0;
	t->cell = 1.f;
	t->mntsize = 10.f;

	t->height = 0;
	t->tex_num = 0;
	t->tex_size = 1.f;

	addRelease(releaseTerrain, (void*)t);
	return t;
}

void loadHeightMap(GTerrain* t, const char* filename)
{
	GImage* hmap = loadImage(filename);
	t->width = hmap->width;

	t->height = new float[t->width * t->width];

	for(int i = 0;i < t->width*t->width;i++)
	{
		t->height[i] =  ((float)(hmap->pixel[(int)(hmap->bpp)*i]) - 127.f) / 255.f * 10.f;
	}

	releaseImage(hmap);
}

/*
GTerrain* generateTerrain(const int width, const int max, const int min)
{
	GTerrain* t = new GTerrain;
	
	// Initialiser le seed
	srand(time((unsigned int)0));

	TERRAIN_CELL = 1.f;

	t->width = width;
	TERRAIN_SIZE = width;

	t->height = new float[width * width];
	t->texture = 0;

	float* map = t->height;

	for(int i=0;i<width*width;i++)
		map[i] = 0.f;

	
	for(int i=0;i<20;i++)
	{
		int x = rand()%width;
		int y = rand()%width;

		int radius = rand() % 10 + 6;
		int hauteur = rand() % (max * (int)TERRAIN_CELL) + min;

		createMountain(map, width, x, y, (float)hauteur, (float)radius);
	}

	// compute normals
	computeNormals(t);

	addRelease(releaseTerrain,(void*)t);

	return t;
}
*/
/*
void createMountain(float* heights,const int width,const int x,const int y,const float height, const float radius)
{
	int starti = x-(int)radius, startj = y-(int)radius;
	if(starti < 0)
		starti=0;
	if(startj < 0)
		startj=0;

	float percent, amount;
	int dx,dy;
	int distance;

	for(int i=starti;i < x+(int)radius;i++)
	{
		if(i >= (int)width)
			break;

		for(int j=startj;j < y+(int)radius;j++)
		{
			if(j >= (int)width)
				break;

			dx = i - x;
			dy = j - y;

			distance = dx * dx + dy  * dy;


			percent = sqrtf((float)distance) * ((F_PI)/radius);
			if(percent>=M_PI)
				percent = M_PI;

			
			amount = (cosf(percent)+1) * (height/2.f);
				heights[i*width + j] += amount;
		}
	}
}
*/
void fillBuffers(GTerrain* t)
{
	float* VertexData = new float[t->width * t->width * 8];

	// Crée un buffer
	glGenBuffersARB(1, &t->VBO);
	glGenBuffersARB(1, &t->IBO);

	// Utilisé ce buffer
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, t->VBO);


	float start = -((float)t->width/2.f) * t->cell;

	for(int i=0;i<t->width;i++)
	{
		for(int j=0;j<t->width;j++)
		{
			VertexData[(i*t->width+j)*8 + 0] = start + (float)j*t->cell;
			VertexData[(i*t->width+j)*8 + 1] = t->height[i * t->width + j] * t->mntsize;
			VertexData[(i*t->width+j)*8 + 2] = start + (float)i*t->cell;
			VertexData[(i*t->width+j)*8 + 3] = 1.f - ((t->mntsize * t->height[i * t->width + j] + (5.f * t->mntsize))/(10.f * t->mntsize));
			VertexData[(i*t->width+j)*8 + 4] = 1.f - ((t->mntsize * t->height[i * t->width + j] + (5.f * t->mntsize))/(10.f*t->mntsize));
			VertexData[(i*t->width+j)*8 + 5] = 1.f - ((t->mntsize * t->height[i * t->width + j] + (5.f*t->mntsize))/(10.f*t->mntsize));

			VertexData[(i*t->width+j)*8 + 6] = (float)i * t->tex_size;
			VertexData[(i*t->width+j)*8 + 7] = (float)j * t->tex_size;
		}
	}

	// Allouer de la mémoire dans la VRAM
	// D'après opengl.org, allouer entre 1MB et 4MB est un bon choix
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, t->width * t->width * 32, (void*)VertexData, GL_STATIC_DRAW_ARB);

	delete[] VertexData;
	// S'occuper de l'Index Buffer
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, t->IBO);

	// Génére les indices
	ushort* indices = new ushort[(t->width-1) * (t->width-1) * 4];
	for(int i = 0;i < t->width-1;i++)
	{
		for(int j = 0;j < t->width-1;j++)
		{
			indices[4 * (i * (t->width - 1) + j) + 0] = (ushort)(i * t->width + j);
			indices[4 * (i * (t->width - 1) + j) + 1] = (ushort)(i * t->width + j + 1);
			indices[4 * (i * (t->width - 1) + j) + 2] = (ushort)(i * t->width + j + t->width + 1);
			indices[4 * (i * (t->width - 1) + j) + 3] = (ushort)(i * t->width + j + t->width);
		}
	}

	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, (t->width-1) * (t->width-1) * 4 * 2, (void*)indices, GL_STATIC_DRAW_ARB);
	t->numIndices = (t->width-1) * (t->width-1) * 4;

	delete[] indices;
}

void render(const GTerrain* t)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, t->VBO);
	
	glActiveTextureARB( GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,t->texture[0]);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
  	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);

	glClientActiveTexture(GL_TEXTURE0_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 32, BUFFER_OFFSET(24));

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(12));

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, t->IBO);

	glDrawElements(GL_QUADS, t->numIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);

	glClientActiveTextureARB(GL_TEXTURE0_ARB);	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void setTexture(GTerrain* t, const char* filename, int stage)
{
	GImage* tex = loadImage(filename);
	t->texture[stage] = allowGLTex(tex);
	releaseImage(tex);

	glBindTexture(GL_TEXTURE_2D, t->texture[stage]);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glBindTexture(GL_TEXTURE_2D, 0);
	
	t->tex_num++;
} 

Vec3f* terrainNormals(float* map, int size)
{
	float cell = (float)(size & 0xF) + 1.f;
	size &= ~(0xF);
	Vec3f* normals = new Vec3f[size * size];

	for(int i=0;i<size;i++)
	{
		for(int j=0;j<size;j++)
		{
			Vec3f left,right,forward,backward;
			if(i-1>=0)
			{
				left = Vec3f(map[(i-1)*size+j] - map[i*size+j],cell,0.f);
			}
			if(size > i+1)
			{
				right = Vec3f(map[i*size+j] - map[(i+1)*size+j],cell,0.f);
			}
			if(j-1>=0)
			{
				forward = Vec3f(0.f,cell,map[i*size-1] - map[i*size+j]);
			}
			if(j+1 < size)
			{
				backward = Vec3f(0.f,cell,map[i*size+j] - map[i*size+j+1]);
			}

			normals[i*size+j] = left + right + forward + backward;
		}
	}

	return normals;
}

void planeTerrain(GTerrain* t, int width)
{
	t->width = width;

	t->height = new float[width * width];

	for(int i=0;i<width*width;i++)
		t->height[i] = 0.f;
}

void releaseTerrain(void* t)
{
	GTerrain* tmp = (GTerrain*)t;
	glDeleteBuffersARB(1, &tmp->VBO);
	glDeleteBuffersARB(1, &tmp->IBO);
	for(int i=0;i<tmp->tex_num;i++)
		glDeleteTextures(1, &tmp->texture[i]);
	delete tmp;
}
