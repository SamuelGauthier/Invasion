#include "invGeom.h"

//------------------------------------------------
// IMFModel
//------------------------------------------------
IMFModel::IMFModel()
{
	numTris = 0;
	numFrames = 0;
	tris = NULL;
}

IMFModel::~IMFModel()
{
	unload();
}

void printvec3f(Vec3f& v)
{
	printf("%g %g %g\n", v.x, v.y, v.z);
}

void printvec2f(Vec2f& v)
{
	printf("%g %g\n", v.x, v.y);
}

void IMFModel::load(FILE* f)
{
	fseek(f, 0, SEEK_END);
	int fileSize = ftell(f);
	rewind(f);

	fread((void*)&numTris, 4, 1, f);
	numFrames = (fileSize-4)/(numTris * sizeof(Triangle));
	isStatic = (numFrames == 1);

	tris = new Triangle[numTris * numFrames];
	fread((void*)&tris[0], sizeof(Triangle), numTris * numFrames, f);

	//bo.setData((void*)tris, (unsigned int)(numTris * 3));
}

void IMFModel::unload()
{
	if(tris)
		delete[] tris;
	tris = NULL;
	numTris = 0;
	numFrames = 0;
}
