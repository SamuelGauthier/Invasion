#include "encoding.h"

int unicode2utf8(unsigned short code, char* dest)
{
	if(code < 0x80)
	{
		dest[0] = (char)code;
		return 1;
	}

	else if(code < 0x800)
	{
		dest[0] = (0xC0 | (code>> 6));
		dest[1] = (0x80 | (code & 0x3f));
		return 2;
	}
	
	dest[0] = (0xE0 | (code >> 12));
	dest[1] = (0x80 | ((code >> 6) & 0x3f));
	dest[2] = (0x80 | (code & 0x3f));
	return 3;
}

unsigned short utf82unicode(const char* bufc)
{
	const unsigned char* buf = (const unsigned char*)bufc;
	if(!(buf[0] & 0x80))
	{
		unsigned short rep = (unsigned short)(buf[0]&0x7f);
		return rep;
	}

	else if(!(buf[0] & 0x20))
	{
		unsigned short a1 = (unsigned short)buf[0];
		unsigned short a2 = (unsigned short)buf[1];
		return (unsigned short)(((a1 & 0x3f) << 6) | (a2 & 0x3f));
	}

	unsigned short a1 = (unsigned short)buf[0];
	unsigned short a2 = (unsigned short)buf[1];
	unsigned short a3 = (unsigned short)buf[2];

	return (unsigned short)(((a1 & 0x1f) << 12) | ((a2 & 0x3f) << 6) | (a3 & 0x3f));
}

const char* nextutf8(const char* buf)
{
	if(!(buf[0] & 0x80)) return buf + 1;
	if(!(buf[0] & 0x20)) return buf + 2;
	return buf + 3;
}
