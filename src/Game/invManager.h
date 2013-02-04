#ifndef INVMANAGER_H
#define INVMANAGER_H

#define MAX_PARAM 32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ppint(x) ((int*)x.get())
#define pint(x) (*ppint(x))
#define pstr(x) ((char*)x.get())

enum MODEL_PARAM
{
	MOD_NAME = 0,
	MOD_CLASS,
	MOD_FUNCTION,
	MOD_WOOD,
	MOD_GOLD,
	MOD_STONE,
	MOD_HP,
	MOD_BUILDTIME,
	MOD_ATTACK,
	MOD_DEFENSE,
	MOD_ALPHA,
	MOD_BOUNDINGX,
	MOD_BOUNDINGZ,
	MOD_RESQUANT,
	MOD_MESHFILE,
	MOD_TEXFILE,
	MOD_CANMOVE,
	MOD_SCALE,
};

enum PARAMETER_TYPE
{
	TYPE_UNDEFINED = 0,
	TYPE_INT = 1,
	TYPE_STRING = 2,
};

class invParameter
{
public:
	invParameter();
	~invParameter();
	
	char name[16];

	void* data;
	int size;
	PARAMETER_TYPE type;

	void set(const char* name, PARAMETER_TYPE type, int count = 1);
	void set(void* data);
	void* get();
};

class invModel
{
public:
	invModel();
	~invModel();

	void set(char* id, char* value);
	bool read(FILE* f);
	void write(FILE* f);

	// model propeties
	invParameter params[MAX_PARAM];

	// model callbacks ( filled in invScript )
	

	// chained list
	invModel* pNext;
};

class invManager
{
public:
	invManager();
	~invManager();

	void open(const char* filename);
	invModel* search(const char* name);
	void remove(const char* name);
	void save(const char* filename);
	char* getList();
	void add(invModel* m);

	invModel* modelList;
	invModel* tail;
};

#endif
