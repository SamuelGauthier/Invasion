//------------------------------------------------
// This Library was exclusivly designed for
// OpenGL
//
// Author: Jyuki
// Date: 11.10.2012
//------------------------------------------------
#ifndef FONT_H
#define FONT_H

// FREETYPE 2 includes
#include <ft2build.h> // contains macros to include the approriate FreeType 2 header files
#include FT_FREETYPE_H // include everything

// OpenGL includes
#include <GL/glew.h>

// Standard C library includes
#include <stdlib.h>
#include <stdio.h>

#include "encoding.h"

class Glyph {
public:
	Glyph ();
	~Glyph ();

	unsigned int size[2];
	unsigned int size2[2];
	unsigned int advance[2];
	unsigned int start[2];
	GLuint tex;
};

class Font {
public:
	Font ();
	~Font ();

	Glyph** glyphs;
	unsigned int numGlyphs;
	int size;

	#ifdef FONT_DEBUG
	Glyph* getGlyph(const unsigned int num) const;
	#else
	inline Glyph* getGlyph(const unsigned int num) const { return glyphs[num]; }
	#endif

	int getSize(const char* str);
};

class FontRenderer {
public:
	FontRenderer ();
	~FontRenderer ();

	void newline();

	void begin();
	void printf(const char* fmt,...);
	void end();

private:
	Font* font;
	float pen[2];
	float start[2];
public:
	inline void setFont(Font* f) { font = f; }
	inline void setPenPos(float x, float y) { pen[0] = x; pen[1] = y; start[0] = x; start[1] = y; }

};

class FontLibrary {
public:
	FontLibrary ();
	~FontLibrary ();

	Font* loadFont(const char* filename, unsigned int ptsize = 12);

private:
	FT_Library library;
	bool checkError(FT_Error error) const;

	Glyph* createGlyph(FT_Face face, unsigned int index, int flags=FT_LOAD_RENDER);

    unsigned int next_p2(unsigned int x);
};

#endif
