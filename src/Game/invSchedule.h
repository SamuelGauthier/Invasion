#ifndef INVSCHEDULE_H
#define INVSCHEDULE_H

#include <lua.hpp>
#include "invList.h"
#include "invEntity.h"
#include "invInput.h"

class invTask
{
public:
	enum TASK_TYPE
	{
		IDLE = 1,
		MOVING_TO_POINT,
		MOVING_TO_ENTITY,
		SLEEP,
		WAIT_FOR_MOVE,
		TURN,
		MOVE,
		WAIT_FOR_SELECTION,
		WAIT_FOR_THREAD,
		QUAD,
		CHANGE_OPACITY,
		TEXTBUFFER,
		WAIT_FOR_KEY,
	};

	invTask();
	~invTask();

	bool update(float elapsedTime);
	invEntity* e;

	void* data[8];
	TASK_TYPE type;
	int thread;
	int unitID;
	bool recall;
	bool background;
	bool pause;
};

#endif
