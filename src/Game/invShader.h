#ifndef INVSHADER_H
#define INVSHADER_H

#include <GL/glew.h>

#include <cstdio>
#include <cstdlib>

class invShader
{
public:
	void load(const char* vname, const char* fname);
	void unload();
	void enable(bool e);

	unsigned int vs, fs, program;
private:
	char* readAll(const char* filename);  // TODO: put this fun somewhere else	
	unsigned int loadShader(const char* fn, GLenum type);
};

#endif
