#include <assert.h>
#include <vector>
#include <string.h>
#include <time.h>
#include "mesh.h"

GTerrain* loadHeightMap(const char* filename,const float fScaleY,const float fScaleXZ)
{
	GTerrain* t = new GTerrain;
	GImage* hmap = loadImage(filename);

	t->width = hmap->width;
	t->length = hmap->height;

	t->cell = 1.f * fScaleXZ;

	t->height = new float[t->width * t->length];
	t->texture = 0;

	for(int i = 0;i < t->length * t->width;i++)
	{
		t->height[i] =  ((float)(hmap->pixel[(int)(hmap->bpp)*i]) - 127.f) / 255.f * 10.f * fScaleY;
	}

	releaseImage(hmap);
	addRelease(releaseTerrain,(void*)t);

	return t;
}

GTerrain* generateTerrain(const uint width, const uint max, const uint min)
{
	GTerrain* t = new GTerrain;
	
	// Initialiser le seed
	srand(time((unsigned int)0));

	t->cell = 5.f;

	t->width = width;
	t->length = width;

	t->height = new float[width * width];
	t->texture = 0;

	float* map = t->height;

	for(uint i=0;i<width*width;i++)
		map[i] = 0.f;

	
	for(uint i=0;i<20;i++)
	{
		uint x = rand()%width;
		uint y = rand()%width;

		uint radius = rand() % 30 + 6;
		uint hauteur = rand() % 50 + 1;

		createMountain(map, width, x, y, (float)hauteur, (float)radius);
	}

	addRelease(releaseTerrain,(void*)t);

	return t;
}

void createMountain(float* heights,const uint width,const int x,const int y,const float height, const float radius)
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

void render(const GTerrain* t)
{
	if(t->texture)
	{
		glEnable(GL_TEXTURE_2D);	
		glBindTexture(GL_TEXTURE_2D,t->texture);

 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	float startX = -(float)t->width/2.f * t->cell;
	float startZ = -(float)t->length/2.f * t->cell;
	for(int i = 0;i < t->width-1;i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(int j = 0;j < t->length;j++)
		{
			if(t->texture)
				glTexCoord2f((float)i*t->tex_size,(float)j*t->tex_size);

			// Plaine
			if(t->height[i*t->length+j] < 10.f)
				glColor3f(0.f,0.7f,0.f);

			else if(t->height[i*t->length+j] < 45.f)
				glColor3f(0.4f,0.4f,0.4f);
			else
				glColor3f(1.f,1.f,1.f);

			glVertex3f((float)i*t->cell + startX, t->height[i * t->length + j], (float)j*t->cell + startZ);
			if(t->texture)
				glTexCoord2f((float)(i+1)*t->tex_size,(float)j*t->tex_size);

			if(t->height[(i+1)*t->length+j] < 10.f)
				glColor3f(0.f,0.7f,0.f);
			else if(t->height[(i+1)*t->length+j] < 45.f)
				glColor3f(0.4f,0.4f,0.4f);
			else
				glColor3f(1.f,1.f,1.f);


			glVertex3f((float)(i+1)*t->cell + startX, t->height[(i+1) * t->length + j], (float)j*t->cell + startZ);
		}
		glEnd();
	}

	if(t->texture)
	{
		glDisable(GL_TEXTURE_2D);
	}
}

void setTexture(GTerrain* t, const char* filename, const float stretch)
{
	GImage* tex = loadImage(filename);
	t->texture = allowGLTex(tex);
	releaseImage(tex);

	t->tex_size = 1.f/stretch;
}

void releaseTerrain(void* t)
{
	GTerrain* tmp = (GTerrain*)t;
	delete[] tmp->height;
	delete tmp;
}

GOBJModel* loadOBJ(const char* filename){
	GOBJModel* m = new GOBJModel;

	// Initialiser
	m->num3 = 0; m->num4 = 0;
	m->texcoords3 = 0;m->texcoords4 = 0;
	m->normals3 = 0;m->normals4 = 0;

	// Charger le fichier obj
	FILE* input = fopen(filename,"r");
	assert(input);

	// Tout les variables qui vont nous servir pour l'entre chargement
	std::vector<Vec3f> vec,norm,vec3, norm3, vec4,norm4;
	std::vector<Vec2f> tex, tex3, tex4;
	uint num_vertices = 0, num_texcoords = 0, num_normals = 0;
	float x,y,z;
	float u,v;

	char line[128];

	while(fgets(line,128,input))
	{
		if(line[0] == 'v')
		{
			// vertices
			if(line[1] == ' ')
			{
				sscanf(line,"v %f %f %f",&x,&y,&z);
				vec.push_back(Vec3f(x,y,z));
				num_vertices++;
			}

			// texcoords
			if(line[1] == 't')
			{
				sscanf(line,"vt %f %f",&u,&v);
				tex.push_back(Vec2f(u,v));
				num_texcoords++;
			}
		}

		else if(line[0] == 'f')
		{
			char* data = line+1;

			// remplacer les slash "/" par des espaces
			replacechar(data,'/',' ');

			// créer une liste de nombre
			uint value = 0, count = 0;
			uint tmp[12];

			while(data)
			{
				sscanf(data," %u",&value);
				tmp[count] = value;

				data = strchr(data+1,' ');
				count++;
			}

			uint index = 0;
			if(count%4)
			{
				for(int i=0;i<3;i++)
				{
					vec3.push_back(vec[tmp[index]-1]);
					index++;

					if(num_texcoords){
						tex3.push_back(tex[tmp[index]-1]);
						index++;
					}

					if(num_normals){
						norm3.push_back(norm[tmp[index]]);
						index++;
					}
				}
				m->num3+=3;
			}

			else
			{
				for(int i=0;i<4;i++)
				{
					vec4.push_back(vec[tmp[index]-1]);
					index++;

					if(num_texcoords){
						tex4.push_back(tex[tmp[index]-1]);
						index++;
					}

					if(num_normals){
						norm4.push_back(norm[tmp[index]]);
						index++;
					}
				}
				m->num4+=4;

			}
		}

	}

	// Copier le tout
	m->vertices3 = new Vec3f[m->num3];
	m->vertices4 = new Vec3f[m->num4];

	for(uint i=0;i<m->num3;i++)
		m->vertices3[i] = vec3[i];
	for(uint i=0;i<m->num4;i++)
		m->vertices4[i] = vec4[i];

	if(num_texcoords)
	{
		m->texcoords3 = new Vec2f[m->num3];
		m->texcoords4 = new Vec2f[m->num4];

		for(uint i=0;i<m->num3;i++)
			m->texcoords3[i] = tex3[i];
		for(uint i=0;i<m->num4;i++)
			m->texcoords4[i] = tex4[i];
	}

	if(num_normals)
	{
		m->normals3 = new Vec3f[m->num3];
		m->normals4 = new Vec3f[m->num4];

		for(uint i=0;i<m->num3;i++)
			m->normals3[i] = norm3[i];
		for(uint i=0;i<m->num4;i++)
			m->normals4[i] = norm4[i];
	}

	fclose(input);
	addRelease(releaseOBJ,(void*)m);

	return m;
}

void render(const GOBJModel* m)
{
	bool isTex = false;

	if(m->texture && (m->texcoords4 || m->texcoords3))
	{
		isTex = true;
	}

	if(isTex)
	{
		glEnable(GL_TEXTURE_2D);	
		glBindTexture(GL_TEXTURE_2D, m->texture);

 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
	}

	glBegin(GL_TRIANGLES);
	
	for(uint i=0;i<m->num3;i++)
	{
		if(isTex)
			glTexCoord2f(m->texcoords3[i].x, m->texcoords3[i].y);
	
		glVertex3f(m->vertices3[i].x, m->vertices3[i].y, m->vertices3[i].z);
	}

	glEnd();

	glBegin(GL_QUADS);

	for(uint i=0;i<m->num4;i++)
	{
		if(isTex)
			glTexCoord2f(m->texcoords4[i].x, m->texcoords4[i].y);

		glVertex3f(m->vertices4[i].x, m->vertices4[i].y, m->vertices4[i].z);
	}

	glEnd();

	if(isTex)
	{
		glDisable(GL_TEXTURE_2D);
	}
}

void releaseOBJ(void* o)
{
	GOBJModel* m = (GOBJModel*)o;
	if(m->vertices3)
		delete[] m->vertices3;
	if(m->vertices4)
		delete[] m->vertices4;
	if(m->texcoords3)
		delete[] m->texcoords3;
	if(m->texcoords4)
		delete[] m->texcoords4;
	if(m->normals3)
		delete[] m->normals3;
	if(m->normals4)
		delete[] m->normals4;
	
	delete m;
}

void setTexture(GOBJModel* m, const char* filename)
{
	m->texture = loadTexture(filename);
}
