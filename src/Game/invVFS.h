#ifndef INVVFS_H
#define INVVFS_H

#include <string.h>

#define s_vfs getVFSHandler()

class invVFS
{
public:
	char buffer[256];

	char* texture(const char* filename);
	char* imfmodel(const char* filename);
	char* script(const char* filename);
	char* iconsandcursors(const char* filename);
};

invVFS* getVFSHandler();

#endif
