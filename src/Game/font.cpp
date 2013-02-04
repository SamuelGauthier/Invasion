//------------------------------------------------
// This Library was exclusivly designed for
// OpenGL
//
// Author: Jyuki
// Date: 11.10.2012
//------------------------------------------------
#include "font.h"

//----------------------------------
// Glyph
//----------------------------------
Glyph::Glyph()
{
	size[0] = 0;
	size[1] = 0;
	advance[0] = 0;
	advance[1] = 0;

	tex = 0;
}

Glyph::~Glyph()
{
	if(tex) glDeleteTextures(1, &tex);
}

//----------------------------------
// Font
//----------------------------------
Font::Font()
{
	glyphs = NULL;
	numGlyphs = 0;
}

Font::~Font()
{
	if(glyphs) {
		for(unsigned int i=0;i<numGlyphs;i++)
		{
			if(glyphs[i]) delete[] glyphs[i];
		}
		delete[] glyphs;
	}
}

#ifdef FONT_DEBUG
Glyph* Font::getGlyph(const unsigned int num) const
{
	if(num >= numGlyphs) {
		fprintf(stderr, "The %d# glyph was not loaded, the upper limit is %d\n", num, numGlyphs-1);
		return NULL;
	}

	if(!glyphs[num])
	{
		fprintf(stderr, "The %d# glyph was not loaded for some reason\n", num);
		return NULL;
	}

	return glyphs[num];
}
#endif

int Font::getSize(const char* str)
{
	int width = 0;
	while(*str)
	{
		unsigned short code = utf82unicode(str);
		width += glyphs[code]->advance[0];

		str = nextutf8(str);
	}

	return width;
}

//----------------------------------
// FontLibrary
//----------------------------------
FontLibrary::FontLibrary()
{
	#ifdef FONT_DEBUG
	printf("Initializing font library...");
	#endif

	FT_Error error;

	error = FT_Init_FreeType(&library);
	if(checkError(error)) return;

	#ifdef FONT_DEBUG
	printf("Done\n");
	#endif
}

FontLibrary::~FontLibrary()
{
	FT_Done_FreeType(library);

	#ifdef FONT_DEBUG
	printf("Released font library.\n");
	#endif
}

Font* FontLibrary::loadFont(const char* filename, unsigned int ptsize)
{
	Font* font = new Font;
	font->glyphs = new Glyph*[256];
	for(int i=0;i<256;i++)
		font->glyphs[i] = NULL;

	FT_Face face;
	FT_Error error;

	#ifdef FONT_DEBUG
	printf("Loading %s...", filename);
	#endif

	error = FT_New_Face( library, filename, 0, &face);
	
	if(checkError(error)) return NULL;
	
	#ifdef FONT_DEBUG
	printf("Done\n");
	printf("numGlyphs : %lu\n", face->num_glyphs);
	#endif


	#ifdef FONT_DEBUG
	printf("Setting point size to %d...", ptsize);
	#endif
	
	error = FT_Set_Char_Size(face, ptsize<<6, ptsize<<6, 96, 96); // 0 means same as corresponding width/height

	if(checkError(error)) return NULL;

	#ifdef FONT_DEBUG
	printf("Done\n");
	#endif
	
	font->size = ptsize; // face->height>>6;
	printf("font->size %d\n", font->size);

	#ifdef FONT_DEBUG
	printf("Loading glyphs...");
	#endif

	for(unsigned int i = 32;i < 256;i++) // ' ' - '~'
	{
		font->glyphs[i] = createGlyph(face, i);

		#ifdef FONT_DEBUG
		if(!font->glyphs[i])
			fprintf(stderr, "\nCould not load the %d glyph\n", i);
		#endif
	}

	printf("H size %d\n", font->glyphs['H']->start[1]);

	#ifdef FONT_DEBUG
	printf("done\n");
	#endif

	FT_Done_Face(face);

	#ifdef FONT_DEBUG
	printf("Glyphs creation finished.\n");
	#endif

    return font;
}

Glyph* FontLibrary::createGlyph(FT_Face face, unsigned int index, int flags)
{
	FT_Error error;
	Glyph* glyph = new Glyph;

	error = FT_Load_Char(face, index, flags);
	if(checkError(error)) return NULL;

	glyph->start[0] = face->glyph->bitmap_left;
	glyph->start[1] = face->glyph->bitmap_top;
	glyph->advance[0] = face->glyph->advance.x>>6;
	glyph->advance[1] = face->glyph->advance.y>>6;
	glyph->size[0] = face->glyph->bitmap.width;
	glyph->size[1] = face->glyph->bitmap.rows;

	glyph->size2[0] = next_p2(glyph->size[0]);
	glyph->size2[1] = next_p2(glyph->size[1]);

	unsigned char* buffer = new unsigned char[glyph->size2[0] * glyph->size2[1]];

	for(unsigned int y=0;y < glyph->size2[1];y++)
	{
		for(unsigned int x=0;x< glyph->size2[0];x++)
		{
			buffer[x + y * glyph->size2[0]] = (x >= glyph->size[0] || y >= glyph->size[1]) ? 0 : face->glyph->bitmap.buffer[x + y * glyph->size[0]];
		}
	}

    glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &glyph->tex);
	glBindTexture(GL_TEXTURE_2D, glyph->tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, glyph->size2[0], glyph->size2[1], 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (void*)buffer);
	delete[] buffer;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

	return glyph;
}

bool FontLibrary::checkError(FT_Error error) const
{
	if(!error) return false;

	fprintf(stderr, "\n");
	if(error == FT_Err_Unknown_File_Format)
		fprintf(stderr, "FTError :: Unknown file format\n");
    if(error == FT_Err_Cannot_Open_Resource)
        fprintf(stderr, "FTError :: Cannot open resource\n");
	else
		fprintf(stderr, "FTError :: error code (%d)\n", error);
	return true;
}

unsigned int FontLibrary::next_p2(unsigned int x)
{
	unsigned int a = 1;
	while(a < x) a *= 2;

	return a;
}

//----------------------------------
// FontRenderer
//----------------------------------
FontRenderer::FontRenderer()
{
	font = NULL;
	pen[0] = pen[1] = 0.f;
	start[0] = start[1] = 0.f;
}

FontRenderer::~FontRenderer()
{
	
}

void FontRenderer::newline()
{
	pen[1] += (float)font->size;
	pen[0] = start[0];
}

void FontRenderer::begin()
{
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
}

void FontRenderer::end()
{
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void FontRenderer::printf(const char* fmt,...)
{
	char buffer[256];

	if(fmt == NULL)
		buffer[0] = '\0';
	else
	{
		va_list ap;
		va_start(ap, fmt);
		vsprintf(buffer, fmt, ap);
		va_end(ap);
	}

	const char* str = buffer;
	unsigned short code = utf82unicode(str);
	while(code != 0)
	{
		if(code == '\n')
		{
			newline();
			str = nextutf8(str);
			code = utf82unicode(str);
			continue;
		}


		Glyph* glyph = font->glyphs[code];
		
		glBindTexture(GL_TEXTURE_2D, glyph->tex);

		float sx = (float)glyph->size[0];
		float sy = (float)glyph->size[1];
		float tx = sx/(float)glyph->size2[0];
		float ty = sy/(float)glyph->size2[1];
		float uy = ((float)font->size - (float)glyph->start[1]);
		glBegin(GL_QUADS);

		glTexCoord2f(0.f, 0.f);
		glVertex3f(pen[0], pen[1] + uy, -1.f);
		glTexCoord2f(tx, 0.f);
		glVertex3f(pen[0] + sx, pen[1] + uy, -1.f);
		glTexCoord2f(tx, ty);
		glVertex3f(pen[0] + sx, pen[1] + uy + sy, -1.f);
		glTexCoord2f(0.f, ty);
		glVertex3f(pen[0], pen[1] + uy + sy, -1.f);

		glEnd();
		pen[0] += (float)glyph->advance[0];

		str = nextutf8(str);
		code = utf82unicode(str);
	}
}
