#ifndef MD5_LOADER_H
#define MD5_LOADER_H

struct GMD5Model
{
};

GMD5Model* loadMD5(const char* filename);
void releaseMD5Model(void* a);

#endif