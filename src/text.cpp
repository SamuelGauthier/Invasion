#define TEXT_SIZE 200.f
//--------------------------------------------------------
// Utilitaire de texte texture-mapped
//
// Ces fonctions fonctionnent uniquement avec les texture-
// mapped fonts générées par Bitmap Font Generator de 
// AngelCode
//--------------------------------------------------------
#include "text.h"

GFont* loadFont(const char* descfile)
{
	// Open file
	GFont* f = new GFont;
	FILE* input = fopen(descfile,"r");
	assert(input);

	GLetter* l = 0;
	char line[128], tgafile[64];
	char* cursor;
	int id;
	
	// initialize
	for(int i=0;i<128;i++)
		f->desc[i].width = 0.f;
	
	// Tranform name
	strcpy(tgafile, descfile);
	cursor = tgafile;
	while(*cursor++);
	strcpy(cursor-4, "tga");

	// Open Image
	GImage* tex = loadImage(tgafile);
	f->w = tex->width;
	f->h = tex->height;

	// Set texture options
	f->texture = allowGLTex(tex);
	glBindTexture(GL_TEXTURE_2D, f->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	releaseImage(tex);

	while(fgets(line, 128, input))
	{
		// char ...
		if(line[0] == 'c' && line[4] == ' ')
		{
			sscanf(line, "char id=%d",&id);
			l = &f->desc[id];

			sscanf(line, "char id=%*u x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d", &l->x, &l->y, &l->width, &l->height, &l->xoffset, &l->yoffset, &l->xadvance);
		}
	}
	
	fclose(input);
	addRelease(releaseFont, (void*)f);

	return f;
}

void render(int posx, int posy, GFont* f, const char* fmt,...)
{
	char text[64];
	va_list ap;
	
	va_start(ap, fmt);
	vsprintf(text, fmt,ap);
	va_end(ap);

	uchar index = 0;
		
	// Enable Alpha Blending
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	// Enable texture
	glBindTexture(GL_TEXTURE_2D, f->texture);
	glBegin(GL_QUADS);

	GLetter* l;
	while(text[index])
	{
		l = &f->desc[(int)text[index]];
		
		if(l->width)
		{
			glTexCoord2f((float)l->x/(float)f->w,
						 ((float)l->y/(float)f->h));
			glVertex2i(posx + l->xoffset, posy + l->yoffset);

			glTexCoord2f((float)(l->x + l->width)/(float)f->w, 
						 ((float)l->y/(float)f->h));
			glVertex2i(posx + l->width + l->xoffset, posy + l->yoffset);
			
			glTexCoord2f((float)(l->x + l->width)/(float)f->w, 
						 ((float)(l->y + l->height)/(float)f->h));
			glVertex2i(posx + l->width + l->xoffset, posy + l->height + l->yoffset);

			glTexCoord2f((float)l->x/(float)f->w,
						(((float)l->y + l->height)/(float)f->h));
			glVertex2i(posx + l->xoffset, posy + l->height + l->yoffset);

			posx += l->xadvance;
		}

		else
		{
			posx += f->desc[43].xadvance;
		}

		index++;
	}

	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void releaseFont(void* f)
{
	GFont* fo = (GFont*)f;
	glDeleteTextures(1, &fo->texture);
	delete fo;
}
