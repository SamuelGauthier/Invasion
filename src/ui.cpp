#define PANELX 550
#define PANELY 0
#define PANELWIDTH 250
#define PANELHEIGHT 600

#include "ui.h"

GControlManager* initControlManager(bool enable)
{
	GControlManager* cm = new GControlManager;
	cm->enable = enable;
	cm->pHead = 0;

	addRelease(releaseControlManager, (void*)cm);
	return cm;
}

GSlideBar* addSlideBar(GControlManager* cm, int min, int max, int value)
{
	GSlideBar* sb = new GSlideBar;
	sb->min = min; sb->max = max;
	sb->value = value;

	GGenericControl* c = new GGenericControl;
	c->type = slide_bar;
	c->ptr = (void*)sb;

	c->pNext = cm->pHead;
	cm->pHead = c;

	return sb;
}

void toggleVisibility(GControlManager* cm)
{
	cm->enable = !cm->enable;
}

void render(GControlManager* cm, GFont* f)
{
	if(cm->enable)
	{
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_SRC_ALPHA);
		glBegin(GL_QUADS);

		glColor4f(0.0f,0.0f,0.0f,0.4f);
		drawSquare(PANELX,PANELY,PANELWIDTH,PANELHEIGHT, -3);

		glEnd();
		glDisable(GL_BLEND);

		int posx = PANELX, posy = PANELY;

		GGenericControl* pCopy = cm->pHead;
		while(pCopy)
		{
			if(pCopy->type==slide_bar)
			{
				glBegin(GL_QUADS);
				GSlideBar* sb = (GSlideBar*)pCopy->ptr;
				float percent = (float)(sb->value - sb->min)/(float)(sb->max - sb->min);
				
				// slidebar
				glColor3f(1.f,1.f,1.f);
				drawSquare(posx + 20,posy + 14, 150, 2,-2);

				// scroller
				glColor3f(0.2f,0.2f,0.7f);
				drawSquare(posx + 15 + (int)(150.f * percent), posy + 8, 10, 14,-1);

				glEnd();

				render(posx + 210, posy + 0, f, "%d", sb->value);
				
			}

			pCopy = pCopy->pNext;

			posy -= 30;
		}
	}
}

void releaseControlManager(void* cm)
{
	GControlManager* tmp = (GControlManager*)cm;

	GGenericControl* todelete=0;
	while(tmp->pHead)
	{
		todelete = tmp->pHead;
		if(todelete->type == slide_bar)
		{
			delete (GSlideBar*)todelete->ptr;
		}
		tmp->pHead = tmp->pHead->pNext;
		
		delete todelete;
	}

	delete tmp;
}

void drawSquare(int x, int y, int width, int height,int depth)
{
	glVertex3i(x,y,depth);
	glVertex3i(x+width,y,depth);
	glVertex3i(x+width,y+height,depth);
	glVertex3i(x,y+height,depth);
}

void setMouse(GControlManager* cm, int x, int y, bool pressed)
{
	if(x > PANELX && y > PANELY && x < (PANELX + PANELWIDTH) && y < (PANELY + PANELHEIGHT))
	{
		GGenericControl* pCopy = cm->pHead;

		int nbblock = (y-PANELY) / 30;
		while(pCopy)
		{	
			if(!nbblock)
			{
				if(pCopy->type == slide_bar && pressed)
				{
					GSlideBar* sb = (GSlideBar*)pCopy->ptr;
					float percentage = (float)(x - PANELX - 20)/150.f;
					if(percentage < 0.f)
					{
						sb->value = sb->min;
					}
					else if(percentage > 1.f)
					{
						sb->value = sb->max;
					}
					else
					{
						sb->value = (sb->max - sb->min) * percentage + sb->min;
					}
				}
				break;
			}

			nbblock--;
			pCopy = pCopy->pNext;
		}
	}
}
