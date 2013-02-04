#include "invTalk.h"
#include "invInput.h"

//------------------------------------------------
// Picking
//------------------------------------------------
int picking(int mousex, int mousey)
{
	Uint8 red;
	glReadPixels(mousex, mousey, 1, 1, GL_RED, GL_UNSIGNED_BYTE, (void*)&red);
	return red;
}

void shootRay(double& objx, double& objy, double& objz, int mousex, int mousey)
{
	GLint viewport[4];
	GLdouble modelview[16], projection[16];
	GLfloat winX, winY, winZ;
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	winX = (GLfloat)mousex;
	winY = (GLfloat)(viewport[3]- mousey);
	glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	gluUnProject(	winX, winY, winZ,
					modelview, projection, viewport,
					&objx, &objy, &objz);
}
