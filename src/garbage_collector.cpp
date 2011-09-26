#include "garbage_collector.h"

GGarbageCollector c;

void addRelease(release_function fn, void* arg)
{
	c.rfnlist.push_back(fn);
	c.arglist.push_back(arg);

}

void releaseAll()
{
	for(unsigned int i=0;i<c.rfnlist.size();i++)
	{
		release_function fn = c.rfnlist[i];
		(*fn)(c.arglist[i]);
	}

	c.rfnlist.clear();
	c.arglist.clear();
}

