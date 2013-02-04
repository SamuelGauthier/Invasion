#ifndef INVBO_H
#define INVBO_H

#define BUFFER_NUM 3

#include <GL/glew.h>

class invBufferObject {
public:
	invBufferObject ();
	~invBufferObject ();

	void setData(void* data, unsigned int count);
	void draw();
private:
	GLuint vbo;
	unsigned int num;
public:
	inline unsigned int getCount() { return num; }

};

#endif
