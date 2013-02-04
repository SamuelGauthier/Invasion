#ifndef INVSCRIPT_H
#define INVSCRIPT_H
#include <string.h>

struct invScript
{
	invScript()
	{
	}

	int entry_point;
	char name[32];
};

#endif
