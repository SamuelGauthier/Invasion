#include <assert.h>
#include <vector>
#include <string.h>
#include "mesh.h"

GTerrain::GTerrain(const char* filename, float fScaleY, float fScaleXZ)
{
	texture = 0;
	tex_size = 1.f;
	OnLoad(filename,fScaleY,fScaleXZ);
}

GTerrain::~GTerrain()
{
	if(height)
		delete[] height;
}

void GTerrain::OnSubdivide()
{
}

void GTerrain::OnLoad(const char* filename, float fScaleY, float fScaleXZ)
{
	GImage* hmap = new GImage(filename);

	width = hmap->width;
	length = hmap->height;

	cellX = 1.f * fScaleXZ;
	cellZ = 1.f * fScaleXZ;

	height = new float[width * length];

	for(int i = 0;i < length * width;i++)
	{
		height[i] =  ((float)(hmap->pixel[(hmap->bpp/8)*i]) - 127.f) / 255.f * 10.f * fScaleY;
	}

	delete hmap;
}

void GTerrain::OnRender() const
{
	if(texture)
	{
		glEnable(GL_TEXTURE_2D);	
		glBindTexture(GL_TEXTURE_2D, texture);

 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	float startX = -(float)width/2.f * cellX;
	float startZ = -(float)length/2.f * cellZ;
	for(int i = 0;i < width-1;i++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(int j = 0;j < length;j++)
		{
			if(texture)
				glTexCoord2f((float)i*tex_size,(float)j*tex_size);

			glVertex3f((float)i*cellX + startX, height[i * length + j], (float)j*cellZ + startZ);
			if(texture)
				glTexCoord2f((float)(i+1)*tex_size,(float)j*tex_size);
			glVertex3f((float)(i+1)*cellX + startX, height[(i+1) * length + j], (float)j*cellZ + startZ);
		}
		glEnd();
	}

	if(texture)
	{
		glDisable(GL_TEXTURE_2D);
	}
}

GOBJModel::GOBJModel(){
	vertices3 = 0; vertices4 = 0;
	normals3 = 0; normals4 = 0;
	texcoords3 = 0; texcoords4 = 0;
	texture = 0;

	num3 = 0; num4 = 0;
}

GOBJModel::GOBJModel(const char* filename){
	vertices3 = 0; vertices4 = 0;
	normals3 = 0; normals4 = 0;
	texcoords3 = 0; texcoords4 = 0;
	num3 = 0; num4 = 0;
	texture = 0;

	OnLoad(filename);
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

void GOBJModel::OnLoad(const char* filename){
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
				num3+=3;
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

GLuint loadTexture(const char* filename)
{
	GLuint texture_id;

	GImage* tex = new GImage(filename);

	glGenTextures(1, &texture_id);
	
	// Dire qu'on travaille avec cette texture
	glBindTexture(GL_TEXTURE_2D, texture_id);

	// Create the GL texture
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGB,
				 tex->width,
				 tex->height,
				 0,
				 GL_RGB,
				 GL_UNSIGNED_BYTE,
				 tex->pixel);

	delete tex;

	return texture_id;
}
