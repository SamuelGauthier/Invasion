#include "invSchedule.h"

//------------------------------------------------
// invTask
//------------------------------------------------
invTask::invTask()
{
	type = IDLE;
	e = NULL;
	unitID = 0;
	recall = false;
	pause = false;
}

invTask::~invTask()
{
	if(type == QUAD)
	{
		invList<invQuadInstance>* list = (invList<invQuadInstance>*)data[1];
		invQuadInstance* qi = (invQuadInstance*)data[0];

		list->rewind();
		while(!list->isEnd())
		{
			if(list->current()->id == qi->id)
			{
				list->erase();
				break;
			}
			list->next();
		}
	}

	if(type == TEXTBUFFER)
	{
		invList<invTextBuffer>* list = (invList<invTextBuffer>*)data[1];
		invTextBuffer* qi = (invTextBuffer*)data[0];

		list->rewind();
		while(!list->isEnd())
		{
			if(list->current()->id == qi->id)
			{
				list->erase();
				break;
			}
			list->next();
		}
	}
}

bool invTask::update(float elapsedTime)
{
	bool done = false;

	float *waiting;
	float* angle;
	float* addangle, *step;
	float* addop;
	Vec3f* addpos;
	Vec3f *pos;
	Vec3f* dest_pos;
	invEntity* dest;
	invQuadInstance* qi;
	invPathFinderMap* map;

	switch(type)
	{
		//***********************************************
		case IDLE:
			done = true;
			break;

		//***********************************************
		case MOVING_TO_POINT:
		case MOVING_TO_ENTITY:
			map = (invPathFinderMap*)data[0];
			dest_pos = (Vec3f*)data[1];
			if(e->followPath(elapsedTime, map, dest_pos))
			{
				if(type == MOVING_TO_ENTITY)
				{
					dest = (invEntity*)data[2];
					e->orientation(dest);
				}
				done = true;
			}
			break;
		//***********************************************
		case SLEEP:
			waiting = (float*)data[0];
			*waiting -= elapsedTime;
			if(*waiting <= 0.f)
				done = true;
			break;

		//***********************************************
		case WAIT_FOR_MOVE:
			if(!e->path)
			{
				done = true;
			}
			break;

		//***********************************************
		case TURN:
			angle = (float*)data[0];
			addangle = (float*)data[1];
			step = (float*)data[2];
			if(*step > 0.f)
			{
				if(elapsedTime > *(step))
					*angle = *angle + (*addangle) * (*step);
				else
					*angle = *angle + (*addangle) * elapsedTime;
				*step = *step - elapsedTime;
			}
			else
				done = true;

			break;

		//***********************************************
		case MOVE:
			pos = (Vec3f*)data[0];
			addpos = (Vec3f*)data[1];
			step = (float*)data[2];
			if(*step > 0.f)
			{
				if(elapsedTime > *(step))
					*pos = *pos + (*addpos) * (*step);
				else
					*pos = *pos + (*addpos) * elapsedTime;
				*step = *step - elapsedTime;
			}
			else
				done = true;

			break;

		//***********************************************
		case WAIT_FOR_SELECTION:
			done = *((bool*)data[0]) == false;
			if(done)
				SDL_ShowCursor(SDL_ENABLE);
			break;
		case CHANGE_OPACITY:
			qi = (invQuadInstance*)data[0];
			addop = (float*)data[1];
			step = (float*)data[2];
			if(*step > 0.f)
			{
				if(elapsedTime > (*step))
					qi->opacity = qi->opacity + (*addop) * (*step);
				else
					qi->opacity = qi->opacity + (*addop) * elapsedTime;
				*step -= elapsedTime;
			}
			else
				done = true;
			break;
		//***********************************************
		case QUAD:
			break;
		case TEXTBUFFER:
			break;
		case WAIT_FOR_KEY:
			if(sinput->keychar)
				done = true;
			break;
		default:
			printf("ERROR : Unknown type of task\n");
			break;
	}

	return done;
}
