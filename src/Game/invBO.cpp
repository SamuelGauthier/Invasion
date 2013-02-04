#include "invBO.h"
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>

invBufferObject ::invBufferObject()
{
	vbo = 0;
	num = 0;
}

invBufferObject::~invBufferObject()
{
	if(vbo) glDeleteBuffersARB(1, &vbo);
}

void invBufferObject::setData(void* data, unsigned int count)
{
	glGenBuffersARB(1, &vbo);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);

	glBufferDataARB(GL_ARRAY_BUFFER_ARB, count * 8 * sizeof(float), data, GL_STATIC_DRAW_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	num = count;
	printf("generating\n");
}
void invBufferObject::draw()
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);

	glVertexPointer(3, GL_FLOAT, 32, NULL);
	glNormalPointer(GL_FLOAT, 32, (char*)NULL + 12);
	glTexCoordPointer(2, GL_FLOAT, 32, (char*)NULL + 24);
	glDrawArrays( GL_TRIANGLES, 0, num);
}
