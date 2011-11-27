#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <vector>
using namespace std;

// pointeur de fonction
typedef void (*release_function)(void*);

struct GGarbageCollector
{
	vector<release_function> rfnlist;
	vector<void*> arglist;
};

void addRelease(release_function fn, void* arg);
void releaseAll();
void releaseElement(void* a);

#endif
