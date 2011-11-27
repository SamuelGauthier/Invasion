#include <assert.h>
#include <vector>
#include <string.h>
#include <time.h>
#include "objloader.h"

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
	int num_vertices = 0, num_texcoords = 0, num_normals = 0;
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

			// normals
			if(line[1] == 'n')
			{
				sscanf(line,"vn %f %f %f",&x,&y,&z);
				norm.push_back(Vec3f(x,y,z));
				num_normals++;
			}
		}

		else if(line[0] == 'f')
		{
			char* data = line+1;

			// remplacer les slash "/" par des espaces
			replacechar(data,'/',' ');

			// créer une liste de nombre
			int value = 0, count = 0;
			int tmp[12];

			while(*data)
			{
				if(*data == ' ' || *data == '\n')
				{
					data++;
					continue;
				}

				sscanf(data,"%i",&value);
				tmp[count] = value;

				while(*data >= '0' && *data <= '9'){data++;}

				count++;
			}

			int index = 0;
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
						norm3.push_back(norm[tmp[index]-1]);
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
						norm4.push_back(norm[tmp[index]-1]);
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

	for(int i=0;i<m->num3;i++)
		m->vertices3[i] = vec3[i];
	for(int i=0;i<m->num4;i++)
		m->vertices4[i] = vec4[i];

	if(num_texcoords)
	{
		m->texcoords3 = new Vec2f[m->num3];
		m->texcoords4 = new Vec2f[m->num4];

		for(int i=0;i<m->num3;i++)
			m->texcoords3[i] = tex3[i];
		for(int i=0;i<m->num4;i++)
			m->texcoords4[i] = tex4[i];
	}
	else
		assert(0);

	if(num_normals)
	{
		m->normals3 = new Vec3f[m->num3];
		m->normals4 = new Vec3f[m->num4];

		for(int i=0;i<m->num3;i++)
			m->normals3[i] = norm3[i];
		for(int i=0;i<m->num4;i++)
			m->normals4[i] = norm4[i];
	}
	else
		assert(0);

	fclose(input);
	addRelease(releaseOBJ,(void*)m);

	return m;
}

void render(const GOBJModel* m)
{
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m->texture);

	if(m->num3)
	{
		glBegin(GL_TRIANGLES);
		
		for(int i=0;i<m->num3;i++)
		{
			glNormal3f(m->normals3[i].x, m->normals3[i].y, m->normals3[i].z); 
			glTexCoord2f(m->texcoords3[i].x, m->texcoords3[i].y);
			glVertex3f(m->vertices3[i].x, m->vertices3[i].y, m->vertices3[i].z);
		}

		glEnd();
	}

	glBegin(GL_QUADS);

	for(int i=0;i<m->num4;i++)
	{
		glNormal3f(m->normals4[i].x, m->normals4[i].y, m->normals4[i].z); 
		glTexCoord2f(m->texcoords4[i].x, m->texcoords4[i].y);
		glVertex3f(m->vertices4[i].x, m->vertices4[i].y, m->vertices4[i].z);
	}

	glEnd();
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
	
	glDeleteTextures(1, &m->texture);
	delete m;
}

void setTexture(GOBJModel* m, const char* filename)
{
	m->texture = loadTexture(filename);

	glBindTexture(GL_TEXTURE_2D, m->texture);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int fillBuffer(GStaticBuffer* sb, GOBJModel* m)
{
	return 0;
}
