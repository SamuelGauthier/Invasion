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

void releaseElement(void* a)
{
	for(unsigned int i=0;i<c.arglist.size();i++)
	{
		if(c.arglist[i] == a)
		{
			(*c.rfnlist[i])(c.arglist[i]);
			c.arglist.erase(c.arglist.begin() + i);
			c.rfnlist.erase(c.rfnlist.begin() + i);
			break;
		}
	}
}