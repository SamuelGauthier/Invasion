#ifndef ENCODING_H
#define ENCODING_H

#include <cstdlib>
#include <cstdio>

int unicode2utf8(unsigned short code, char* dest);
unsigned short utf82unicode(const char* buf);
const char* nextutf8(const char* buffer);
#endif
