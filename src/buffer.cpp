#include "buffer.h"

GStaticBuffer* initStaticBuffer(int sizeVBO, int sizeIBO)
{
	GStaticBuffer* sb = new GStaticBuffer;
	
	glGenBuffers(1, &sb->VBO);
	glGenBuffers(1, &sb->IBO);

	sb->num = 0;

	glBindBuffer(GL_ARRAY_BUFFER, sb->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeVBO, NULL, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sb->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIBO, NULL, GL_STATIC_DRAW);

	addRelease(releaseStaticBuffer,(void*)sb);

	return sb;
}

int addStaticBuffer(GStaticBuffer* sb, Vec3f* vertices, Vec3f* normals, Vec2f* texcoords, int numVert,  ushort* indices, int numInd, int mode)
{
	float* VertexData = new float[numVert * 8];

	for(int i=0;i<numVert;i++)
	{	
		VertexData[i*8 + 0] = vertices[i].x;
		VertexData[i*8 + 1] = vertices[i].y;
		VertexData[i*8 + 2] = vertices[i].z;
		VertexData[i*8 + 3] = normals[i].x;
		VertexData[i*8 + 4] = normals[i].y;
		VertexData[i*8 + 5] = normals[i].z;
		VertexData[i*8 + 6] = texcoords[i].x;
		VertexData[i*8 + 7] = texcoords[i].y;
	}

	int index = addStaticBuffer(sb, VertexData, numVert, indices, numInd, mode);
	delete[] VertexData;

	return index;
}

int addStaticBuffer(GStaticBuffer* sb, float* VertexData, int numVert, ushort* indices, int numInd, int mode)
{
	if(sb->num >= 16)
		return -1;

	glBindBuffer(GL_ARRAY_BUFFER, sb->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, sb->start[sb->num*2], numVert * 32, (void*)VertexData);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sb->IBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sb->start[sb->num*2+1], numInd * 2, (void*)indices);

	// préparer la prochaine entrée
	if(sb->num < 15)
	{
		sb->start[(sb->num + 1) * 2] = sb->start[sb->num * 2] + numVert * 32;
		sb->start[(sb->num + 1) * 2 + 1] = sb->start[sb->num * 2 + 1] + numInd * 2;
	}
	sb->mode[sb->num] = mode;
	sb->num++;

	return (sb->num-1);
}

void setTexture(GStaticBuffer* sb, const char* filename, int index)
{
	if(index >= 0 && index < 16)
	{
		GImage* tex = loadImage(filename);
		sb->tex[index] = allowGLTex(tex);
		releaseImage(tex);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, sb->tex[index]);

 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glDisable(GL_TEXTURE_2D);

	}
}

void beginRender(GStaticBuffer* sb)
{
	glEnable(GL_TEXTURE_2D);
	glBindBuffer(GL_ARRAY_BUFFER, sb->VBO);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 32, BUFFER_OFFSET(12));
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 32, BUFFER_OFFSET(24));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sb->IBO);
}

void prepareObject(GStaticBuffer* sb, int index)
{
	glBindTexture(GL_TEXTURE_2D, sb->tex[index]);
	glClientActiveTexture(GL_TEXTURE0);

	sb->curcount = sb->start[index * 2 + 3] - sb->start[index * 2 + 1];
	sb->curstart = sb->start[index * 2 + 1];
	sb->curend = sb->curstart + sb->curcount;
}

void render(GStaticBuffer* sb, int index)
{
	glDrawRangeElements(sb->mode[index],
						sb->curstart,
						sb->curend,
						sb->curcount,
						GL_UNSIGNED_SHORT,
						BUFFER_OFFSET(0));
}

void endRender()
{
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glDisable(GL_TEXTURE_2D);
}

void releaseStaticBuffer(void* sb)
{
	GStaticBuffer* tmp = (GStaticBuffer*)sb;

	glDeleteBuffers(1, &tmp->VBO);
	glDeleteBuffers(1, &tmp->IBO);

	delete tmp;
}
