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
		t->height[i] =  ((float)(hmap->pixel[(int)(hmap->bpp)*i]) - 127.f) / 255.f * 5.f;
	}

	releaseImage(hmap);
}

float interpolate(float a, float b, float percent)
{
	if(percent==0.f)
	    return a;
	if(percent==1.f)
	    return b;
	
	float v1 = 3.f * powf(1.f-percent, 2.f) - 2.f*powf(1.f-percent, 3.f);
	float v2 = 3.f * powf(percent, 2.f) - 2.f *powf(percent, 3.f);
	
	return a*v1 + b*v2;
}

// a----b
// |	|
// c----d
void linear_interpolation(GTerrain* trn, int a, int b, int c, int d)
{
	float v1 = trn->height[a];
	float v2 = trn->height[b];
	float v3 = trn->height[c];
	float v4 = trn->height[d];

	int z = a / trn->width, x  = a % trn->width;

	float percentx_step = 1.f/(float)((b-a));
	float percentz_step = 1.f/(float)((c-a)/trn->width);

	float percentz = 0.f;
	for(int i = z;i < (c-a)/trn->width + z + 1;i++)
	{
		float percentx = 0.f;
		for(int j = x;j < b-a + x + 1;j++)
		{
			float i1 = interpolate(v1, v2, percentx);
			float i2 = interpolate(v3, v4, percentx);
			trn->height[i * trn->width + j] = interpolate(i1, i2, percentz);

			percentx += percentx_step;
		}
		percentz += percentz_step;
	}
}

void randomGen(GTerrain* t, int freq, int height, int chance)
{
	int chunk = (t->width-1)/(freq+1);
	
	int a = 0;
	int start_b = 0;

	if(rand() % 1000 < chance)
		t->height[0] = (float)(rand()%height);
	for(int i = 0;i < freq + 1;i++)
	{
		int b = (i+1) * chunk * t->width;
		
		if(i == freq)
			b = t->width * (t->width - 1);

		if(rand() % 1000 < chance)
			t->height[b] = (float)(rand()%height);
		
		start_b = b;
		for(int j = 0;j < freq + 1;j++)
		{
			int c = a + chunk;
			int d = b + chunk;

			if(j == freq)
			{
				c = a + t->width - (a % t->width) - 1;
				d = start_b + t->width - 1;
			}

			if(i == 0 && (rand() % 1000 < chance))
				t->height[c] = (float)(rand()%height);

			if(rand() % 1000 < chance)
				t->height[d] = (float)(rand()%height);
		
			linear_interpolation(t, a, c, b, d);

			a = c;
			b = d;
		}
		a = start_b;
	}
}

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
			float coeff = (float)(rand()%101)/100.f;
			VertexData[(i*t->width+j)*8 + 0] = start + (float)j*t->cell;
			VertexData[(i*t->width+j)*8 + 1] = t->height[i*t->width + j];
			VertexData[(i*t->width+j)*8 + 2] = start + (float)i*t->cell;
			VertexData[(i*t->width+j)*8 + 3] = 1.f - (t->height[i*t->width + j]/10.f);
			VertexData[(i*t->width+j)*8 + 4] = 1.f - (t->height[i*t->width + j]/10.f);
			VertexData[(i*t->width+j)*8 + 5] = 1.f - (t->height[i*t->width + j]/10.f);
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
	ushort* indices = new ushort[(t->width-1) * (t->width-1) * 6];
	for(int i = 0;i < t->width-1;i++)
	{
		for(int j = 0;j < t->width-1;j++)
		{
			indices[6 * (i * (t->width - 1) + j) + 0] = (ushort)(i * t->width + j);
			indices[6 * (i * (t->width - 1) + j) + 1] = (ushort)(i * t->width + j + 1);
			indices[6 * (i * (t->width - 1) + j) + 2] = (ushort)(i * t->width + j + t->width + 1);
			indices[6 * (i * (t->width - 1) + j) + 3] = (ushort)(i * t->width + j);
			indices[6 * (i * (t->width - 1) + j) + 4] = (ushort)(i * t->width + j + t->width + 1);
			indices[6 * (i * (t->width - 1) + j) + 5] = (ushort)(i * t->width + j + t->width);
		}
	}

	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, (t->width-1) * (t->width-1) * 6 * 2, (void*)indices, GL_STATIC_DRAW_ARB);
	t->numIndices = (t->width-1) * (t->width-1) * 6;

	delete[] indices;
}

void render(const GTerrain* t)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, t->VBO);
	
	glActiveTextureARB( GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,t->texture[0]);

	glClientActiveTexture(GL_TEXTURE0_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 32, BUFFER_OFFSET(24));

	
	glActiveTextureARB( GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,t->texture[1]);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_COLOR);
	
	glClientActiveTexture(GL_TEXTURE1_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 32, BUFFER_OFFSET(24));
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
	glColorPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(12));

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, t->IBO);

	glDrawElements(GL_TRIANGLES, t->numIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

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
/*
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
}*/

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
