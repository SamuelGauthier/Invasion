#include "console.h"

GConsole* initConsole()
{
	GConsole* c = new GConsole;
	c->show_percentage = 0;
	c->show = false;

	addRelease(releaseConsole, (void*)c);
	return c;
}

void toggleConsole(GConsole* c)
{
	c->show = !c->show;
}

void render(GConsole* c, GFont* f)
{
	if(c->show || c->show_percentage)
	{
		if(c->show_percentage != 200 && c->show)
		{
			c->show_percentage+=25;
		}
		
		else if(c->show_percentage != 0 && !c->show)
		{
			c->show_percentage-=40;
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_SRC_ALPHA);

		glColor4f(0.f,0.f,0.f,0.1f);
		glBegin(GL_QUADS);

		glVertex3i(0, 0, -1);
		glVertex3i(800, 0, -1);
		glVertex3i(800, c->show_percentage, -1);
		glVertex3i(0, c->show_percentage, -1);

		glEnd();
		glDisable(GL_BLEND);

		glColor3f(1.f,1.f,1.f);

		render(0, c->show_percentage - 200, f, "Salut");
	}
}

void releaseConsole(void* c)
{
	GConsole* tmp = (GConsole*)c;
	delete tmp;
}
