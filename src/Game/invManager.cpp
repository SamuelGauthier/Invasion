#include "invManager.h"

//------------------------------------------------
// invParameter
//------------------------------------------------
invParameter::invParameter()
{
	name[0] = '\0';

	data = NULL;
	size = 0;
	type = TYPE_UNDEFINED;
}

invParameter::~invParameter()
{
	if(data)
	{
		if(type == TYPE_INT)
			delete (int*)data;
		else if(type == TYPE_STRING)
			delete[] (char*)data;
	}
}

void invParameter::set(const char* n, PARAMETER_TYPE t, int c)
{
	type = t;
	size = c;

	if(t == TYPE_INT)
	{
		if(data)
			delete (int*)data;
		data = new int;
		*(int*)data = 0;
		size *= 4;
	}

	if(t == TYPE_STRING)
	{
		if(data)
			delete[] (char*)data;
		data = new char[c];
		*(char*)data = '\0';
	}

	strncpy(name, n, 16);
}

void invParameter::set(void* d)
{
	if(type == TYPE_INT)
	{
		int* dest = (int*)data; int* src = (int*)d;
		*dest = *src;
	}
	if(type == TYPE_STRING)
		strncpy((char*)data, (char*)d, size);
}

void* invParameter::get()
{
	return data;
}

//------------------------------------------------
// invModel
//------------------------------------------------
invModel::invModel()
{
	pNext = NULL;

	params[MOD_NAME].set("Name", TYPE_STRING, 32);
	params[MOD_CLASS].set("Class", TYPE_INT);
	params[MOD_FUNCTION].set("Function", TYPE_INT);
	params[MOD_WOOD].set("Wood", TYPE_INT);
	params[MOD_GOLD].set("Gold", TYPE_INT);
	params[MOD_STONE].set("Stone", TYPE_INT);
	params[MOD_HP].set("HP", TYPE_INT);
	params[MOD_BUILDTIME].set("Buildtime", TYPE_INT);
	params[MOD_ATTACK].set("Attack", TYPE_INT);
	params[MOD_DEFENSE].set("Defense", TYPE_INT);
	params[MOD_ALPHA].set("Alpha", TYPE_INT);
	params[MOD_BOUNDINGX].set("BoundingX", TYPE_INT);
	params[MOD_BOUNDINGZ].set("BoundingY", TYPE_INT);
	params[MOD_RESQUANT].set("Resquant", TYPE_INT);
	params[MOD_MESHFILE].set("Meshfile", TYPE_STRING, 32);
	params[MOD_TEXFILE].set("Texfile", TYPE_STRING, 32);
	params[MOD_CANMOVE].set("canMove", TYPE_INT);
	params[MOD_SCALE].set("scale", TYPE_INT);
}

invModel::~invModel()
{
}

void invModel::set(char* id, char* value)
{
	for(int i=0;i<32;i++) {
		if(!strcmp(id, params[i].name)){
			if(params[i].type == TYPE_INT)
			{
				int integer = atoi(value);
				params[i].set((void*)&integer);
			}
			else if(params[i].type == TYPE_STRING)
			{
				params[i].set((void*)value);
			}
			break;
		}
	}
}

bool invModel::read(FILE* f)
{
	bool written = false;
	char line[128];
	char id[32];
	char value[64];
	int found;
	while(fgets(line, 128, f) && !feof(f))
	{
		found = sscanf(line, "%s = %[a-zA-Z0-9_. ]", id, value);
		if(found == 1){
			if(!strcmp(id, "end"))
				break;
		}

		if(found == 2){
			written = true;
			set(id, value);
		}
	}
	return written;
}

void invModel::write(FILE* f)
{
	for(int i=0;i<32;i++)
	{
		if(strlen(params[i].name) && params[i].data)
		{
			if(params[i].type == TYPE_INT)
				fprintf(f, "%s = %d\n", params[i].name, (*(int*)params[i].get()));
			else if(params[i].type == TYPE_STRING)
				fprintf(f, "%s = %s\n", params[i].name, (char*)params[i].get());
		}
	}
	fprintf(f, "\nend\n\n");
}
//------------------------------------------------
// invManager
//------------------------------------------------
invManager::invManager()
{
	modelList = NULL;
	tail = NULL;
}

invManager::~invManager()
{
}

void invManager::open(const char* filename)
{
	FILE* data = fopen(filename, "r");
	if(!data)
	{
		fprintf(stderr, "ERROR : Could not open %s\n", filename);
		return;
	}

	invModel* m = NULL;
	while(!feof(data))
	{
		m = new invModel();
		if(!m->read(data)) {
			delete m;
			break;
		}
		add(m);
	}

	fclose(data);
}

void invManager::add(invModel* m)
{
	if(!modelList)
		modelList = m;
	else
		tail->pNext = m;

	tail = m;

}

invModel* invManager::search(const char* name)
{
	invModel* m = modelList;
	while(m)
	{
		if(!strcmp((char*)m->params[MOD_NAME].get(), name))
			break;
		m = m->pNext;
	}
	return m;
}

void invManager::remove(const char* name)
{
	invModel* previous = NULL;
	invModel* m = modelList;

	while(m) {
		if(!strcmp((char*)m->params[MOD_NAME].get(), name)) {
			if(!previous) {
				modelList = m->pNext;
				delete m;
			}
			else {
				previous->pNext = m->pNext;
				delete m;
			}
			break;
		}
		previous = m;
		m = m->pNext;
	}
}

void invManager::save(const char* filename)
{
	FILE* data = fopen(filename, "w");
	invModel* m = modelList;
	while(m){
		m->write(data);
		m = m->pNext;
	}
	fclose(data);
}

char* invManager::getList()
{
	char* buffer = new char[128];
	char* str = buffer;
	char* name;
	*str = '\0';

	invModel* copy = modelList;
	while(copy)
	{
		name = (char*)copy->params[MOD_NAME].get();
		strcat(str, name);
		str[strlen(name)] = '\n'; // replace '\0' by '\n'
		str += (strlen(name) + 1);
		*str = '\0';
		copy = copy->pNext;
	}

	return buffer;
}
