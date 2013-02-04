#include "invVFS.h"

invVFS vfs;

//------------------------------------------------
// invVFS
//------------------------------------------------
char* invVFS::texture(const char* filename)
{
	strcpy(buffer, "Textures/");
	strcat(buffer, filename);

	return &buffer[0];
}

char* invVFS::imfmodel(const char* filename)
{
	strcpy(buffer, "3DModels/");
	strcat(buffer, filename);

	return &buffer[0];
}

char* invVFS::script(const char* filename)
{
	strcpy(buffer, "Scripts/");
	strcat(buffer, filename);

	return &buffer[0];
}

char* invVFS::iconsandcursors(const char* filename)
{
	strcpy(buffer, "IconsandCursors/");
	strcat(buffer, filename);

	return &buffer[0];
}

invVFS* getVFSHandler()
{
	return &vfs;
}
