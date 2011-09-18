#include <assert.h>
#include <vector>
#include <string.h>
#include "mesh.h"

GTerrain* loadHeightMap(const char* filename,const float fScaleY,const float fScaleXZ)
{
	GTerrain* t = new GTerrain;
	GImage* hmap = loadImage(filename);

	t->width = hmap->width;
	t->length = hmap->height;

	t->cellX = 1.f * fScaleXZ;
	t->cellZ = 1.f * fScaleXZ;

	t->height = new float[t->width * t->length];
	t->texture = 0;

	for(int i = 0;i < t->length * t->width;i++)
	{
		t->height[i] =  ((float)(hmap->pixel[(hmap->bpp/8)*i]) - 127.f) / 255.f * 10.f * fScaleY;
	}

	releaseImage(hmap);
	addRelease(releaseTerrain, (void*)t);

	return t;
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

	float startX = -(float)t->width/2.f * t->cellX;
	float startZ = -(float)t->length/2.f * t->cellZ;
	for(int i = 0;i < t->width-1;i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(int j = 0;j < t->length;j++)
		{
			if(t->texture)
				glTexCoord2f((float)i*t->tex_size,(float)j*t->tex_size);

			glVertex3f((float)i*t->cellX + startX, t->height[i * t->length + j], (float)j*t->cellZ + startZ);
			if(t->texture)
				glTexCoord2f((float)(i+1)*t->tex_size,(float)j*t->tex_size);
			glVertex3f((float)(i+1)*t->cellX + startX, t->height[(i+1) * t->length + j], (float)j*t->cellZ + startZ);
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

GOBJModel::~GOBJModel(){
	if(vertices3)
		delete[] vertices3;
	if(vertices4)
		delete[] vertices4;
	if(texcoords3)
		delete[] texcoords3;
	if(texcoords4)
		delete[] texcoords4;
	if(normals3)
		delete[] normals3;
	if(normals4)
		delete[] normals4;
}

GOBJModel* loadOBJ(const char* filename){
	OBJModel* m = new OBJModel;

	// Initialiser
	m->num3 = 0; m->num4 = 0;

	// Charger le fichier obj
	FILE* input = fopen(filename,"r");
	assert(input);

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
					m->vec3.push_back(vec[tmp[index]-1]);
					index++;

					if(num_texcoords){
						m->tex3.push_back(tex[tmp[index]-1]);
						index++;
					}

					if(num_normals){
						m->norm3.push_back(norm[tmp[index]]);
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
				num4+=4;

			}
		}

	}

	// Copier le tout
	vertices3 = new Vec3f[num3];
	vertices4 = new Vec3f[num4];

	for(uint i=0;i<num3;i++)
		vertices3[i] = vec3[i];
	for(uint i=0;i<num4;i++)
		vertices4[i] = vec4[i];

	if(num_texcoords)
	{
		texcoords3 = new Vec2f[num3];
		texcoords4 = new Vec2f[num4];

		for(uint i=0;i<num3;i++)
			texcoords3[i] = tex3[i];
		for(uint i=0;i<num4;i++)
			texcoords4[i] = tex4[i];
	}

	if(num_normals)
	{
		normals3 = new Vec3f[num3];
		normals4 = new Vec3f[num4];

		for(uint i=0;i<num3;i++)
			normals3[i] = norm3[i];
		for(uint i=0;i<num4;i++)
			normals4[i] = norm4[i];
	}

	fclose(input);
}

void GOBJModel::OnRender() const{
	bool isTex = false;

	if(texture && (texcoords4 || texcoords3))
	{
		isTex = true;
	}

	if(isTex)
	{
		glEnable(GL_TEXTURE_2D);	
		glBindTexture(GL_TEXTURE_2D, texture);

 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
	}

	glBegin(GL_TRIANGLES);
	
	for(uint i=0;i<num3;i++)
	{
		if(isTex)
			glTexCoord2f(texcoords3[i].x, texcoords3[i].y);
	
		glVertex3f(vertices3[i].x, vertices3[i].y, vertices3[i].z);
	}

	glEnd();

	glBegin(GL_QUADS);

	for(uint i=0;i<num4;i++)
	{
		if(isTex)
			glTexCoord2f(texcoords4[i].x, texcoords4[i].y);

		glVertex3f(vertices4[i].x, vertices4[i].y, vertices4[i].z);
	}

	glEnd();

	if(isTex)
	{
		glDisable(GL_TEXTURE_2D);
	}


}
*/
