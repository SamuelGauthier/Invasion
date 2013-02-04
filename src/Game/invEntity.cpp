#define DIV0_HACK 0.5f
#define REPULSION_FORCE 8.f
#define POWER_CONC 1.5f
#define FRAME_DELAY (1.f/14.f)
#define MASS_MULT 2.f

#include "invEntity.h"

//----------------------------------
// invObject
//----------------------------------
invObject::invObject()
{
	id = 0;
	opacity = 1.f;
	visible = true;

	pos = Vec3f(0.f, 0.f, 0.f);
	rot = Vec3f(0.f, 0.f, 0.f);
	scale = Vec3f(1.f, 1.f, 1.f);
}

invObject::~invObject()
{
}

//------------------------------------------------
// invQuad
//------------------------------------------------
invQuad::invQuad()
{
	tex = 0;
	in2D = false;
}

invQuad::~invQuad()
{
	if(tex) glDeleteTextures(1, &tex);
}

//------------------------------------------------
// invQuadInstance
//------------------------------------------------
invQuadInstance::invQuadInstance(invQuad* quad)
{
	q = quad;
}

invQuadInstance::~invQuadInstance()
{
}

void invQuadInstance::draw()
{
	glColor4f(1.f, 1.f, 1.f, opacity);
	glBindTexture(GL_TEXTURE_2D, q->tex);

	glPushMatrix();

	if(rot.x != 0.f) glRotatef(rot.x, 1.f, 0.f, 0.f);
	if(rot.y != 0.f) glRotatef(rot.y, 0.f, 1.f, 0.f);
	if(rot.z != 0.f) glRotatef(rot.z, 0.f, 0.f, 1.f);

	glBegin(GL_QUADS);

	glTexCoord2f(0.f, 0.f); glVertex3f(pos.x, pos.y, pos.z);
	glTexCoord2f(1.f, 0.f); glVertex3f(pos.x+q->w, pos.y, pos.z);
	glTexCoord2f(1.f, 1.f); glVertex3f(pos.x+q->w, pos.y+q->h, pos.z);
	glTexCoord2f(0.f, 1.f); glVertex3f(pos.x, pos.y+q->h, pos.z);

	glEnd();

	glPopMatrix();
}


//------------------------------------------------
// invEntity
//------------------------------------------------
invEntity::invEntity(ModelDef* m)
{
	mdef = m;
	state = 0;

	if(mdef->scale)
		scale = Vec3f(mdef->scale, mdef->scale, mdef->scale);

	mobile = mdef->canMove;

	time_interval = 0.f;
	StartFrame = 0;
	EndFrame = 0;
	CurFrame = 0;

	path = NULL;
}

invEntity::~invEntity()
{
	deletePath();
}

void invEntity::setMesh(IMFModel* m)
{
	mesh = m;

	interpolatedDataV = new Vec3f[mesh->numTris*3];
	interpolatedDataN = new Vec3f[mesh->numTris*3];
	interpolatedDataT = new Vec2f[mesh->numTris*3];

	for(int i=0;i<mesh->numTris;i++)
	{
		interpolatedDataV[i*3] = m->tris[i].v0;
		interpolatedDataN[i*3] = m->tris[i].n0;
		interpolatedDataT[i*3] = m->tris[i].t0;

		interpolatedDataV[i*3+1] = m->tris[i].v1;
		interpolatedDataN[i*3+1] = m->tris[i].n1;
		interpolatedDataT[i*3+1] = m->tris[i].t1;

		interpolatedDataV[i*3+2] = m->tris[i].v2;
		interpolatedDataN[i*3+2] = m->tris[i].n2;
		interpolatedDataT[i*3+2] = m->tris[i].t2;
	}

}

bool invEntity::followPath(float elapsedTime, invPathFinderMap* map, Vec3f* dest)
{
	// TODO : == sign for Vec*f
	if(!path)
	{
		return true;
	}

	else if(map->isFreeCell(path->index) || map->getIndex(pos.x, pos.z) == path->index)
	{
		map->occupiedByMobileUnit[map->getIndex(pos.x, pos.z)] = false;
		move(elapsedTime);

		map->occupiedByMobileUnit[map->getIndex(pos.x, pos.z)] = true;
		return false;
	}

	deletePath();
	
	// find path
	path = searchPath(pos.x, pos.z, dest->x, dest->z, map);
	
	invPathNode* copy = path;
	while(copy)
	{
		copy = copy->pNext;
	}

	return false;
}

void invEntity::deletePath()
{
	while(path)
	{
		invPathNode* tmp = path;
		path = path->pNext;
		delete tmp;
	}
}


void invEntity::move(float elapsedTime)
{
	Vec2f delta;

	while(path && elapsedTime > 0.f)
	{
		delta.x = path->x - pos.x;
		delta.y = path->y - pos.z;
		float length = sqrtf(delta.x * delta.x + delta.y * delta.y);


		float mult = (ENTITY_SPEED * elapsedTime)/length;
		if(mult >= 1.f)
		{
			pos.x = path->x;
			pos.z = path->y;

			elapsedTime -= length/ENTITY_SPEED;
			path = path->pNext;
		}
		
		else
		{
			pos.x += delta.x*mult;
			pos.z += delta.y*mult;
			elapsedTime = 0.f;
		}
	}

	orientation(delta.x, delta.y);
}

void invEntity::orientation(float dx, float dy)
{
	float angle = atanf(-dy/dx);
	if(dx < 0.f)
		angle += F_PI;
	rot.y = angle * (180.f/F_PI) + 90.f;
}

void invEntity::orientation(invEntity* e)
{
	orientation(e->pos.x - pos.x, e->pos.z - pos.z);
}

void invEntity::animate(float elapsed)
{
	time_interval += elapsed;
	if(time_interval >= FRAME_DELAY)
	{
		time_interval -= FRAME_DELAY;
		if(++CurFrame > EndFrame) CurFrame = StartFrame;
	}

	int NextFrame = CurFrame+1;
	if(NextFrame > EndFrame) NextFrame = StartFrame;

	if(CurFrame != NextFrame)
	{
		interpolate(time_interval/FRAME_DELAY, CurFrame, NextFrame);
	}
}

void invEntity::interpolate(float elapsedTime, int frame_a, int frame_b)
{
	if(elapsedTime > 1.f)
		elapsedTime = 1.f;
	float a = 1.f - elapsedTime;
	float b = elapsedTime;
	int start_a = frame_a * mesh->numTris;
	int start_b = frame_b * mesh->numTris;
	for(int i=0;i<mesh->numTris;i++)
	{
		interpolatedDataV[i*3] = mesh->tris[start_a+i].v0 * a + mesh->tris[start_b+i].v0 * b;
		interpolatedDataN[i*3] = mesh->tris[start_a+i].n0 * a + mesh->tris[start_b+i].n0 * b;
		interpolatedDataT[i*3] = mesh->tris[start_a+i].t0 * a + mesh->tris[start_b+i].t0 * b;

		interpolatedDataV[i*3+1] = mesh->tris[start_a+i].v1 * a + mesh->tris[start_b+i].v1 * b;
		interpolatedDataN[i*3+1] = mesh->tris[start_a+i].n1 * a + mesh->tris[start_b+i].n1 * b;
		interpolatedDataT[i*3+1] = mesh->tris[start_a+i].t1 * a + mesh->tris[start_b+i].t1 * b;

		interpolatedDataV[i*3+2] = mesh->tris[start_a+i].v2 * a + mesh->tris[start_b+i].v2 * b;
		interpolatedDataN[i*3+2] = mesh->tris[start_a+i].n2 * a + mesh->tris[start_b+i].n2 * b;
		interpolatedDataT[i*3+2] = mesh->tris[start_a+i].t2 * a + mesh->tris[start_b+i].t2 * b;
	}
}

void invEntity::draw()
{
	glPushMatrix();
	glTranslatef(pos.x, pos.y + 0.01f, pos.z);

	// TODO: optimize this
	//glRotatef(rot.x, 1.f, 0.f, 0.f);
	glRotatef(rot.y, 0.f, 1.f, 0.f);
	//glRotatef(rot.z, 0.f, 0.f, 1.f);

	// scale
	glScalef(scale.x, scale.y, scale.z);

	glTexCoordPointer(2, GL_FLOAT, 0, interpolatedDataT);
	glNormalPointer(GL_FLOAT, 0, interpolatedDataN);
	glVertexPointer(3, GL_FLOAT, 0, interpolatedDataV);

	glDrawArrays(GL_TRIANGLES, 0, mesh->numTris * 3);
	// mesh->bo.draw();
	glPopMatrix();
}

//------------------------------------------------
// invTextBuffer
//------------------------------------------------
invTextBuffer::invTextBuffer()
{
	for(int i=0;i<256;i++)
		str[i] = '\0';
	font = NULL;
}


invTextBuffer::~invTextBuffer()
{

}

void invTextBuffer::setFont(Font* f)
{
	font = f;
}

void invTextBuffer::setText(const char* src)
{
	strcpy(str, src);
}
