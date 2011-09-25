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
	GFont* f = new GFont;
	FILE* input = fopen(descfile,"r");
	assert(input);

	char line[128];
	
	uint id, px, py, width, height, tex_width, tex_height;
	while(fgets(line, 128, input))
	{
		// char ...
		if(line[0] == 'c' && line[4] == ' ')
		{
			sscanf(line, "char id=%u x=%u y=%u width=%u height=%u", &id, &px, &py, &width, &height);

			f->desc[id].x = (float)px/(float)tex_width; f->desc[id].y = (float)py/(float)tex_height;
			f->desc[id].width = (float)width/(float)tex_width; f->desc[id].height = (float)height/(float)tex_height;
		}

		// page...
		else if(line[0] == 'p')
		{
			char filename[64]; 
			sscanf(line, "page id=%*u file=\"%s\"", filename);
			
			// Petit bug, le guillemet reste à la fin
			for(int i=0;filename[i];i++)
			{
				if(filename[i] == '\"')
				{
					filename[i] = '\0';
					break;
				}
			}

			GImage* tex = loadImage(filename);
			tex_width = tex->width;
			tex_height = tex->height;

			f->texture = allowGLTex(tex);
			releaseImage(tex);
		}
	}

	fclose(input);
	addRelease(releaseFont, (void*)f);

	return f;
}

void render(GFont* f, const char* s)
{
	float posx = 0.f, posy = 0.f;
	uchar index = 0;

	// Enable texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, f->texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBegin(GL_QUADS);

	while(s[index])
	{
		GLetter* l = &f->desc[s[index]];
		
		glTexCoord2f(l->x, l->y);
		glVertex3f(posx, posy, 0.f);

		glTexCoord2f(l->x + l->width, l->y);
		glVertex3f(posx + l->width * 10.f, posy, 0.f);

		glTexCoord2f(l->x + l->width, l->y + l->height);
		glVertex3f(posx + l->width * 10.f, posy + l->height * 10.f, 0.f);

		glTexCoord2f(l->x, l->y + l->height);
		glVertex3f(posx, posy + l->height * 10.f, 0.f);

		posx += l->width * 10.f;

		index++;
	}

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void releaseFont(void* f)
{
	GFont* fo = (GFont*)f;
	delete fo;
}
