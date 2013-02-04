#ifndef INVTALK_H
#define INVTALK_H

#include "invMath.h"
#include "invTex.h"
#include "invList.h"
#include "invVFS.h"
#include <GL/glew.h>
#include <string.h>
#include <lua.hpp>

typedef void (*event_function)(void);

// Picking
int picking(int mousex, int mousey);
void shootRay(double& objx, double& objy, double& objz, int mousex, int mousey);
#endif
