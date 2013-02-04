#include "invRTS.h"

invList<invTask> tasks;
invList<invEntity> entities;
invList<invQuadInstance> quadinstances;
invList<ModelDef> mps;
invList<invQuad> quads;
invList<invTextBuffer> textbuffers;

invQuadInstance* mquad[128] = {0};
int mquad_count = 0;
invQuadInstance* mquad_previous_in = NULL; 

invEntity* selection_patron = NULL;
invEntity* mouse_over = NULL;

bool selection_unit = false;
Vec3f selection_place;
int selection_defid = 0;
bool selection_success = false;
bool selection_place_available = false;
int unitDefIDCounter = 1;
int unitIDCounter = 1;
int quadInstanceIDCounter = 1;
int textBufferIDCounter = 1;

invSoundSystem ssys;
invMap* drawgrid = NULL;
invPathFinderMap* pfgrid = NULL;
bool draw_grid = false;
bool draw_map = false;
float terrainX, terrainZ;
bool ignore_next_elapsedTime = false;
int window_width, window_height;

// selection variables
invEntity* selectedl[16];
invEntity* selectedr;
invEntity* selected[16];
int nb_selected;
int nb_selectedl;
int nb_selectedr;

// minimap
float mm_width = 228.f, mm_height = 228.f;
float mm_posx = 33.f, mm_posy = 33.f;
bool mm_visible = true;

// font
FontLibrary flib;
invList<Font> fonts;
FontRenderer fdraw;

GLuint fow = 0;
bool pause_game = false;
bool picking_enable = false;
//------------------------------------------------
// invRTSGame
//------------------------------------------------
invRTSGame::invRTSGame()
{
	for(int i=0;i<4;i++)
		resource[i] = 0;
	nb_selected = 0;
	terrain_point = false;
	areaSel = false;

	id_counter = 1;
}

invRTSGame::~invRTSGame()
{
	glDeleteTextures(1, &fow);
	entities.clean();
	mps.clean();

	lua_close(mainL);
}

invObject* invRTSGame::findObject(unsigned int id)
{
	unsigned int header = id>>29;
	switch(header)
	{
		case ENTITY_IDH:
			return findEntity(id);
		case QUAD_INSTANCE_IDH:
			return findQuadInstance(id);
		case TEXT_BUFFER_IDH:
			return findTextBuffer(id);
		default:
			break;
	}

	fprintf(stderr, "This id (%d) was not found\n", (header<<29)|id);
	return NULL;
}

void invRTSGame::setWindowResolution(int w, int h)
{
	window_width = w;
	window_height = h;
}

void invRTSGame::setTaskInfo(invTask* t, lua_State* L)
{
	lua_getglobal(L, "activeThread");
	if(lua_isnil(L, -1))
	{
		printf("setTaskInfo : called a function outside a thread\n");
		lua_pop(L, 1);

		t->thread = 0;
		t->background = false;
	}

	else
	{
		lua_getfield(L, -1, "background");
		t->background = (bool)lua_toboolean(L, -1);
		lua_getfield(L, -2, "id");
		t->thread = (int)lua_tonumber(L, -1);
		lua_getfield(L, -3, "unitid");
		if(!lua_isnil(L, -1))
			t->unitID = (int)lua_tonumber(L, -1);
		else
			t->unitID = 0;

		lua_pop(L, 4);
	}
}

int invRTSGame::CreateUnitDef(lua_State* L)
{
	// prototype ( props )  all has to do with display
	lua_getfield(L, 1, "name");
	lua_getfield(L, 1, "meshfile");
	lua_getfield(L, 1, "texfile");
	lua_getfield(L, 1, "boundingx");
	lua_getfield(L, 1, "boundingz");
	lua_getfield(L, 1, "alpha");
	lua_getfield(L, 1, "scale");
	lua_getfield(L, 1, "canMove");
	lua_getfield(L, 1, "selectable");
	lua_getfield(L, 1, "sight");

	const char* meshfile = lua_tostring(L, 3), *texfile = lua_tostring(L, 4);
	FILE* f = fopen(s_vfs->imfmodel(meshfile), "rb");
	if(!f)
	{
		printf("ERROR : %s not found\n", meshfile);
		return 0;
	}

	GLuint tex = loadTexture(s_vfs->texture(texfile));
	if(!tex)
	{
		printf("ERROR : %s not found\n", texfile);
		return 0;
	}

	ModelDef* def = new ModelDef;
	def->mesh = new IMFModel;
	def->mesh->load(f);
	fclose(f);

	def->tex = tex;
	def->id = unitDefIDCounter++;
	def->bx = lua_tonumber(L, 5);
	def->bz = lua_tonumber(L, 6);

	if(lua_isnil(L, 7)) def->alpha = 0;	
	else def->alpha = lua_tonumber(L, 7);
	
	if(lua_isnil(L, 8)) def->scale = 1.f;
	else def->scale = lua_tonumber(L, 8);

	if(lua_isnil(L, 9)) def->canMove = false;
	else def->canMove = lua_toboolean(L, 9);

	if(lua_isnil(L, 10)) def->selectable = true;
	else def->selectable = lua_toboolean(L, 10);

	if(lua_isnil(L, 11)) def->sight = 0;
	else def->sight = lua_tonumber(L, 11);

	strncpy(def->name, lua_tostring(L, 2), 32);

	mps.addBack(def);

	int num = lua_gettop(L) - 1;
	lua_pop(L, num);

	lua_pushnumber(L, def->id);
	return 1;
}

int invRTSGame::togglePicking(lua_State* L)
{
	picking_enable = !picking_enable;
	return 0;
}

void invRTSGame::command()
{
	if(pause_game) return;

	if(selection_unit)
	{
		Vec3f pos;

		pfgrid->getPos(pos.x, pos.z, pfgrid->getIndex(terrainX + selection_patron->mdef->bx, terrainZ + selection_patron->mdef->bz));

		selection_place_available = pfgrid->isAvailable(pos.x - selection_patron->mdef->bx, pos.z - selection_patron->mdef->bz, pos.x + selection_patron->mdef->bx, pos.z + selection_patron->mdef->bz);

		selection_patron->pos.x = pos.x;
		selection_patron->pos.z = pos.z;

		if(sinput->mousebutton == SDL_BUTTON_RIGHT && sinput->mousestate == 1)
		{
			selection_success = false;
			selection_unit = false;
		}

		if(sinput->mousebutton == SDL_BUTTON_LEFT && sinput->mousestate == 2 && selection_place_available)
		{
			selection_place = selection_patron->pos;
			selection_success = true;
			selection_unit = false;
		}
	}

	if(sinput->keychar == 't')
		draw_grid = !draw_grid;

	quad_monitoring();

	// 

	if(sinput->mousebutton == SDL_BUTTON_LEFT && sinput->mousestate == 1)
	{
		lua_getglobal(mainL, "SendSignal");
		lua_pushnumber(mainL, 33);
		lua_pushnumber(mainL, 0);

		lua_call(mainL, 2, 0);
	}

	if(sinput->mousebutton == SDL_BUTTON_LEFT && sinput->mousestate == 2)
	{
		lua_getglobal(mainL, "SendSignal");
		lua_pushnumber(mainL, 34);
		lua_pushnumber(mainL, 0);

		lua_call(mainL, 2, 0);
	}

	if(sinput->mousebutton == SDL_BUTTON_RIGHT && sinput->mousestate == 1)
	{
		lua_getglobal(mainL, "SendSignal");
		lua_pushnumber(mainL, 35);
		lua_pushnumber(mainL, 0);

		lua_call(mainL, 2, 0);
	}

	if(sinput->mousebutton == SDL_BUTTON_RIGHT && sinput->mousestate == 2)
	{
		lua_getglobal(mainL, "SendSignal");
		lua_pushnumber(mainL, 36);
		lua_pushnumber(mainL, 0);

		lua_call(mainL, 2, 0);
	}

	if(sinput->keychar)
	{
		lua_getglobal(mainL, "SendSignal");
		lua_pushnumber(mainL, 37);
		lua_pushnumber(mainL, 0);
		lua_pushnumber(mainL, (int)sinput->keychar);
		printf("key %d\n", sinput->keychar);

		lua_call(mainL, 3, 0);
	}

}

void invRTSGame::move(float elapsedTime)
{
	if(draw_map)
		updateFow();

	if(pause_game) return;

	if(ignore_next_elapsedTime)
	{
		elapsedTime = 0.f;
		ignore_next_elapsedTime = false;
	}
	//------------------------------------------------
	// Animations
	//------------------------------------------------
	entities.rewind();
	while(!entities.isEnd())
	{
		invEntity* e = entities.current();
		if(e->mesh->numFrames > 1)
			e->animate(elapsedTime);
		entities.next();
	}

		//------------------------------------------------
	// Update resources
	//------------------------------------------------
	/*
	lua_getglobal(mainL, "food");	
	resource[0] = lua_tonumber(mainL, 1);
	lua_pop(mainL, 1);
	lua_getglobal(mainL, "wood");	
	resource[1] = lua_tonumber(mainL, 1);
	lua_pop(mainL, 1);
	lua_getglobal(mainL, "gold");	
	resource[2] = lua_tonumber(mainL, 1);
	lua_pop(mainL, 1);
	lua_getglobal(mainL, "stone");	
	resource[3] = lua_tonumber(mainL, 1);
	lua_pop(mainL, 1);
	*/

}

void invRTSGame::quad_monitoring()
{
	//------------------------------------------------
	// Quad Monitoring
	//------------------------------------------------
	int mousex = sinput->mousex, mousey = sinput->mousey;
	invQuadInstance* mquad_now_in = NULL;
	for(int i=0;i<mquad_count;i++)
	{
		invQuadInstance* qi = mquad[i];
		if(!qi)
			printf("MQUADMONITORING : quad not found\n");
		if(mousex > qi->pos.x && mousex < qi->pos.x + qi->q->w &&
		   mousey > qi->pos.y && mousey < qi->pos.y + qi->q->h)
		{
			mquad_now_in = qi;
			break;
		}
	}

	if(mquad_now_in != mquad_previous_in)
	{
		if(mquad_previous_in)
		{
			// SendSignal out
			lua_getglobal(mainL, "SendSignal");
			lua_pushnumber(mainL, 302);
			lua_pushnumber(mainL, 0);
			lua_pushnumber(mainL, mquad_previous_in->id);
			lua_call(mainL, 3, 0);
		}

		if(mquad_now_in)
		{
			// SendSignal in
			lua_getglobal(mainL, "SendSignal");
			lua_pushnumber(mainL, 301);
			lua_pushnumber(mainL, 0);
			lua_pushnumber(mainL, mquad_now_in->id);
			lua_call(mainL, 3, 0);
		}
	}
	mquad_previous_in = mquad_now_in;

}

int invRTSGame::updateQuadZ(lua_State* L)
{
	invQuadInstance* qi = findQuadInstance(lua_tonumber(L, 1));
	if(qi)
	{
		quadinstances.seek(qi);
		quadinstances.remove();
		quadinstances.addBack(qi);
	}
	
	return 0;
}

void invRTSGame::beginFX(int alpha, float opacity)
{
	if(alpha == 2)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5);
	}

	else if(alpha == 1 || opacity < 1.f)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(1.f, 1.f, 1.f, opacity);
	}
}

void invRTSGame::endFX(int alpha)
{
	if(alpha == 2)
		glDisable(GL_ALPHA_TEST);
	else if(alpha == 1)
		glDisable(GL_BLEND);
}

void invRTSGame::drawEntities()
{
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	entities.rewind();
	while(!entities.isEnd())
	{
		
		invEntity* e = entities.current();
		unsigned char visub = pfgrid->visible[pfgrid->getIndex(e->pos.x, e->pos.z)];

		glBindTexture(GL_TEXTURE_2D, e->tex);
		beginFX(e->mdef->alpha, e->opacity);

		glColor3ub(visub, visub, visub);
		
		e->draw();

		endFX(e->mdef->alpha);
		entities.next();
	}

	glDisable(GL_COLOR_MATERIAL);
	
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(selection_unit)
	{
		if(selection_place_available)
			glColor4f(1.f, 1.f, 1.f, 0.5f);
		else
			glColor4f(1.0f, 0.f, 0.f, 0.5f);
		glBindTexture(GL_TEXTURE_2D, selection_patron->tex);
		selection_patron->draw();
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDisable(GL_BLEND);

}

void invRTSGame::drawQuads()
{
	//------------------------------------------------
	// Draw the selection circles
	//------------------------------------------------
	glColor3f(1.f, 1.f, 1.f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	quadinstances.rewind();
	while(!quadinstances.isEnd())
	{
		invQuadInstance* q = quadinstances.current();
		if(q->visible && !q->q->in2D)
			q->draw();
		quadinstances.next();
	}

	
	glBindTexture(GL_TEXTURE_2D, circle);
	glColor3f(1.f, 1.f, 1.f);
	for(int i=0;i < nb_selectedl;i++)
	{
		glPushMatrix();
		glTranslatef(selectedl[i]->pos.x, 0.02f, selectedl[i]->pos.z);
		glRotatef(90.f, 1.f, 0.f, 0.f);

		drawTexturedSquare(selectedl[i]->mdef->bx * 1.5f, selectedl[i]->mdef->bz * 1.5f);
		glPopMatrix();
	}


	if(mouse_over)
	{
		glPushMatrix();
		glTranslatef(mouse_over->pos.x, 0.02f, mouse_over->pos.z);
		glRotatef(90.f, 1.f, 0.f, 0.f);

		glColor4f(1.f, 1.f, 1.f, 0.5f);
		drawTexturedSquare(mouse_over->mdef->bx * 1.5f, mouse_over->mdef->bz * 1.5f);
		glPopMatrix();
	}

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	if(draw_grid)
	{
		glBegin(GL_QUADS);
		// draw the grid
		for(int i=0;i<pfgrid->countx;i++)
		{
			for(int j=0;j<pfgrid->county;j++)
			{
				if(pfgrid->occupied[i * pfgrid->county + j] ||
				pfgrid->occupiedByMobileUnit[i * pfgrid->county + j])
					glColor3f(1.f, 0.f, 0.f);
				else
					glColor3f(0.f, 1.f, 0.f);

				glVertex3f(pfgrid->startx + (float)i * pfgrid->cellx,
						   0.2f, 
						   pfgrid->starty + (float)j * pfgrid->celly);
				glVertex3f(pfgrid->startx + (float)(i+1) * pfgrid->cellx,
						   0.2f, 
						   pfgrid->starty + (float)j * pfgrid->celly);
				glVertex3f(pfgrid->startx + (float)(i+1) * pfgrid->cellx,
						   0.2f, 
						   pfgrid->starty + (float)(j+1) * pfgrid->celly);
				glVertex3f(pfgrid->startx + (float)i * pfgrid->cellx,
						   0.2f, 
						   pfgrid->starty + (float)(j+1) * pfgrid->celly);
			}
		}

		glEnd();
	}

	glEnable(GL_TEXTURE_2D);
}

void invRTSGame::draw2D()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);

	quadinstances.rewind();
	while(!quadinstances.isEnd())
	{
		invQuadInstance* q = quadinstances.current();
		if(q->visible && q->q->in2D)
		{
			q->draw();
		}
		quadinstances.next();
	}

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glColor3f(1.f, 1.f, 1.f);
	fdraw.begin();
	textbuffers.rewind();
	while(!textbuffers.isEnd())
	{
		invTextBuffer* tb = textbuffers.current();
		fdraw.setPenPos(tb->pos.x, tb->pos.y);
		fdraw.setFont(tb->font);
		fdraw.printf("%s", tb->str);
		textbuffers.next();
	}

	fdraw.end();
}

int invRTSGame::myprint(lua_State* L)
{
	printf("%s\n", lua_tostring(L, 1));
	return 0;
}
void invRTSGame::selection(int height)
{
	if(pause_game || !picking_enable) return;

	mouse_over = NULL;

	// draw a black terrain
	if(draw_map)
	{
		glColor3ub(0, 0, 0);
		drawgrid->draw(fow, false);
		terrainPoint();
	}

	if(selection_unit) return;

	if(sinput->mousebutton == SDL_BUTTON_LEFT && sinput->mousestate == 1)
	{
		startMouseSelX = sinput->mousex;
		startMouseSelY = sinput->mousey;
		startAreaSelX = terrainX;
		startAreaSelZ = terrainZ;
		
		areaSel = true;
	}

	else if(sinput->mousebutton == SDL_BUTTON_LEFT && sinput->mousestate == 2)
	{
		startMouseSelX = endMouseSelX;
		startMouseSelY = endMouseSelY;
		endAreaSelX = terrainX;
		endAreaSelZ = terrainZ;
		areaSel = false;
	}

	if(areaSel)
	{
		endMouseSelX = sinput->mousex;
		endMouseSelY = sinput->mousey;
		endAreaSelX = terrainX;
		endAreaSelZ = terrainZ;
	}

	int deltaX = endMouseSelX - startMouseSelX;
	int deltaY = endMouseSelY - startMouseSelY;
	if(areaSel  && (deltaX * deltaY) > 5 * 5)
	{
		selectEntityInArea();
	}

	// TODO : Optimize this !!
	else{
		Uint8 limitcolor = 1;
		nb_selected = 0;

		entities.rewind();
		while(!entities.isEnd())
		{
			invEntity* e = entities.current();

			if(e->mdef->selectable)
			{
				glColor3ub(limitcolor++, 0, 0);
				// draw the box
				glPushMatrix();
				glTranslatef(e->pos.x, 0.f, e->pos.z);
				drawCube(e->mdef->bx, 2.5f, e->mdef->bz);
				glPopMatrix();
			}

			entities.next();
		}

		int selected_color = picking(sinput->mousex, height -  sinput->mousey);

		limitcolor = 1;
		entities.rewind();
		while(!entities.isEnd())
		{
			if(entities.current()->mdef->selectable && selected_color == limitcolor++)
			{
				nb_selected = 1;
				mouse_over = entities.current();
				break;
			}
			entities.next();
		}
		
		if(sinput->mousebutton == SDL_BUTTON_LEFT && sinput->mousestate == 1) {
			if(nb_selected)
			{
				nb_selectedl = 1;
				selectedl[0] = mouse_over;
			}
		}

		if(sinput->mousebutton == SDL_BUTTON_RIGHT && sinput->mousestate == 1) {
			nb_selectedr = nb_selected;
			if(nb_selectedr) selectedr = mouse_over;

			if(nb_selectedr)
			{
				lua_pushnumber(mainL, selectedr->id);
				lua_setglobal(mainL, "target_unit");
			}

			else
			{
				lua_pushnil(mainL);
				lua_setglobal(mainL, "target_unit");
			}
		}
	}

	// notice lua
	lua_pushnumber(mainL, nb_selectedl);
	lua_setglobal(mainL, "nb_selected");

	if(nb_selectedl)
	{
		lua_getglobal(mainL, "selected_units");
		for(int i=0;i<nb_selectedl;i++)
		{
			lua_pushnumber(mainL, i+1);
			lua_pushnumber(mainL, selectedl[i]->id);
			lua_rawset(mainL, -3);
		}
		lua_pop(mainL, 1);
	}

	
}

void invRTSGame::terrainPoint()
{
	double objx, objy, objz;
	shootRay(objx, objy, objz, sinput->mousex, sinput->mousey);

	terrainX = (float)objx;
	terrainZ = (float)objz;
	// printf("terrainX : %g terrainZ : %g\n", terrainX, terrainZ);

	if(sinput->mousebutton == SDL_BUTTON_LEFT && sinput->mousestate == 2) {
		// notice lua
		lua_pushnumber(mainL, nb_selectedl);
		lua_setglobal(mainL, "nb_selected");

		if(nb_selectedl)
		{
			lua_getglobal(mainL, "selected_units");
			lua_pushnumber(mainL, 1);
			lua_pushnumber(mainL, selectedl[0]->id);
			lua_rawset(mainL, -3);
			lua_pop(mainL, 1);
		}
	}
}

void invRTSGame::selectEntityInArea()
{
	nb_selectedl = 0;
	float minX = Math::minf(startAreaSelX, endAreaSelX);
	float maxX = Math::maxf(startAreaSelX, endAreaSelX);
	float minZ = Math::minf(startAreaSelZ, endAreaSelZ);
	float maxZ = Math::maxf(startAreaSelZ, endAreaSelZ);
	entities.rewind();
	while(!entities.isEnd())
	{
		invEntity* e = entities.current();
		if(e->mobile) {
			if(	e->pos.x + e->mdef->bx > minX && e->pos.x - e->mdef->bx < maxX 
			&&	e->pos.z + e->mdef->bz > minZ && e->pos.z - e->mdef->bz < maxZ)
			{
				selectedl[nb_selectedl++] = e;
			}
		}
		entities.next();
	}
}

invEntity* invRTSGame::findEntity(unsigned int id)
{
	entities.rewind();
	while(!entities.isEnd())
	{
		if(entities.current()->id == id) return entities.current();
		entities.next();
	}
	printf("ERROR : Entity not found\n");
	return NULL;
}

void invRTSGame::initScriptEngine()
{
	mainL = lua_open();
	luaL_openlibs(mainL);
	
	// TODO: a macro for lua_register
	lua_register(mainL, "SleepA", sleep);
	lua_register(mainL, "Turn", turn);
	lua_register(mainL, "Move", move);
	lua_register(mainL, "MouseClickDown", mouseclickdown);
	lua_register(mainL, "MouseClickUp", mouseclickup);
	lua_register(mainL, "RegisterUnitDef", CreateUnitDef);
	lua_register(mainL, "RegisterUnit", registerUnit);
	lua_register(mainL, "UnregisterUnit", unregisterUnit);
	lua_register(mainL, "MoveToPoint", moveToPoint);
	lua_register(mainL, "MoveToEntity", moveToEntity);
	lua_register(mainL, "KillTasks", killTasks);
	lua_register(mainL, "KillTasksWithThread", killTasksWithThread);
	lua_register(mainL, "WaitForMoveA", waitForMove);
	lua_register(mainL, "CreateQuad", CreateQuad);
	lua_register(mainL, "LoadFont", loadFont);
	lua_register(mainL, "CreateTextQuad", CreateTextQuad);
	lua_register(mainL, "CreateQuadInstance", CreateQuadInstance);
	lua_register(mainL, "DeleteQuadInstance", deleteQuadInstance);
	lua_register(mainL, "SetQuadSize", SetQuadSize);
	lua_register(mainL, "SelectPlaceA", SelectPlace);
	lua_register(mainL, "GetSelectedResult", getSelectedResult);
	lua_register(mainL, "SetAnim", SetAnim);
	lua_register(mainL, "GetPos", getPos);
	lua_register(mainL, "SetPos", setPos);
	lua_register(mainL, "OccupArea", occupArea);
	lua_register(mainL, "LoadMap", loadMap);
	lua_register(mainL, "GetPixel", getPixel);
	lua_register(mainL, "UnloadMap", unloadMap);
	lua_register(mainL, "CreateTerrain", createTerrain);
	lua_register(mainL, "LoadTerrainTextures", loadTerrainTextures);
	lua_register(mainL, "DestroyTerrain", destroyTerrain);
	lua_register(mainL, "InsertMQuad", insertMQuad);
	lua_register(mainL, "DeleteMQuad", deleteMQuad);
	lua_register(mainL, "Resize", resize);
	lua_register(mainL, "ChangeOpacity", changeOpacity);
	lua_register(mainL, "ResetElapsedTime", resetElapsedTime);
	lua_register(mainL, "GetResolution", getResolution);
	lua_register(mainL, "SetQuadVisible", setQuadVisible);
	lua_register(mainL, "GetMouseRayPos", getMouseRayPos);
	lua_register(mainL, "SetMinimap", setMinimap);
	lua_register(mainL, "UpdateQuadZ", updateQuadZ);
	lua_register(mainL, "CreateTextBuffer", CreateTextBuffer);
	lua_register(mainL, "DeleteTextBuffer", DeleteTextBuffer);
	lua_register(mainL, "SetText", setText);
	lua_register(mainL, "LoadFont", loadFont);

	lua_register(mainL, "LoadSound", loadSound);
	lua_register(mainL, "PlaySound", playSound);

	lua_register(mainL, "SetClearColor", setClearColor);
	lua_register(mainL, "myprint", myprint);
	lua_register(mainL, "WaitForKeyA", waitForKey);
	lua_register(mainL, "TogglePause", togglePause);
	lua_register(mainL, "TogglePicking", togglePicking);
}


int invRTSGame::togglePause(lua_State* L)
{
	pause_game = !pause_game;

	tasks.rewind();
	while(!tasks.isEnd())
	{
		tasks.current()->pause = pause_game;
		tasks.next();
	}

	return 0;
}

int invRTSGame::waitForKey(lua_State* L)
{
	invTask* t = new invTask;
	t->type = invTask::WAIT_FOR_KEY;
	t->recall = true;

	setTaskInfo(t, L);
	tasks.add(t);

	return 0;
}

void invRTSGame::insertQuadInstance(invQuadInstance* qi)
{
	float previous_depth = -10000.f;
	while(!quadinstances.isEnd())
	{
		if(quadinstances.current()->pos.z >= qi->pos.z && qi->pos.z >= previous_depth)
		{
			quadinstances.insert(qi);
			return;
		}
		previous_depth = quadinstances.current()->pos.z;
		quadinstances.next();
	}
	quadinstances.addBack(qi);
}

void invRTSGame::runScripts()
{
	DIR* d = opendir(s_vfs->script(""));
	struct dirent* dir;

	invList<invScript> scripts;
	while((dir = readdir(d)))
	{
		// ignore directories
		if(dir->d_type != DT_REG) continue;
		
		// ignore hidden files
		if(dir->d_name[0] == '.') continue;

		if(luaL_dofile(mainL, s_vfs->script(dir->d_name)))
		{
			fprintf(stderr, "%s\n", lua_tostring(mainL, -1));
			break;
		}

		// read infos
		lua_getglobal(mainL, "info");
		if(lua_isnil(mainL, 1))
		{
			fprintf(stderr, "%s has no infos\n", dir->d_name);
			continue;
		}

		lua_getfield(mainL, 1, "name");
		lua_getfield(mainL, 1, "entry_point");

		invScript* script = new invScript;
		
		strncpy(script->name, lua_tostring(mainL, 2), 32);
		if(!lua_isnil(mainL, 3) && strlen(lua_tostring(mainL, 3)) != 0){
			lua_getglobal(mainL, lua_tostring(mainL, 3));
			script->entry_point = lua_ref(mainL, LUA_REGISTRYINDEX);
		}

		else{
			script->entry_point = 0;
		}


		int num = lua_gettop(mainL);
		lua_pop(mainL, num);

		scripts.add(script);
	}

	scripts.rewind();
	while(!scripts.isEnd())
	{
		if(scripts.current()->entry_point)
		{
			lua_rawgeti(mainL, LUA_REGISTRYINDEX, scripts.current()->entry_point);
			lua_call(mainL, 0, 0);
		}
		scripts.erase();
	}

	lua_getglobal(mainL, "SendSignal");
	lua_pushnumber(mainL, 1);
	lua_pushnumber(mainL, 0);

	lua_call(mainL, 2, 0);

}

int invRTSGame::sleep(lua_State* L)
{
	invTask* t = new invTask;
	t->type = invTask::SLEEP;
	float* countdown = new float((float)lua_tonumber(L, 1));
	t->data[0] = (void*)countdown;
	t->recall = true;

	setTaskInfo(t, L);
	tasks.add(t);

	return 0;
}

int invRTSGame::waitForMove(lua_State* L)
{
	invTask* t = new invTask;
	t->type = invTask::WAIT_FOR_MOVE;
	int* countdown = new int(10);
	t->data[0] = (void*)countdown;
	t->recall = true;

	setTaskInfo(t, L);
	
	int unitID = lua_tonumber(L, 1);
	t->e = findEntity(unitID);
	t->unitID = unitID;

	tasks.addBack(t);

	return 0;
}

void invRTSGame::initTerrainDim(int count, float size)
{
	
}

void invRTSGame::processTasks(float elapsedTime)
{
	if(ignore_next_elapsedTime)
	{
		elapsedTime = 0.f;
		ignore_next_elapsedTime = false;
	}

	tasks.rewind();
	while(!tasks.isEnd())
	{
		if(!tasks.current()->pause && tasks.current()->update(elapsedTime))
		{
			// TODO: call the thread to say that we're finished with this
			if(tasks.current()->recall)
			{
				int thread = tasks.current()->thread;
				tasks.erase();
				lua_getglobal(mainL, "WakeUp");
				lua_pushnumber(mainL, thread);
				lua_call(mainL, 1, 0);
			}
			else
			{
				tasks.erase();
			}
			continue;
		}
		tasks.next();
	}
}

int invRTSGame::turn(lua_State* L)
{
	float speed = (float)lua_tonumber(L, 2);
	if(speed <= 0.f){
		printf("Turn :: negative or speed speed give\n");
		return 0;
	}

	Vec3f* rot = NULL;
	invTask* t = new invTask;
	t->type = invTask::TURN;
	t->unitID = (int)lua_tonumber(L, 4);
	rot = &findObject(t->unitID)->rot;

	int axis = (int)lua_tonumber(L, 3);
	float* angle = NULL;
	if(axis == 1) // x
		angle = &rot->x;
	else if(axis == 2) // y
		angle = &rot->y;
	else if(axis == 3) // z
		angle = &rot->z;
	
	float total = (float)lua_tonumber(L, 1);

	float* addangle = new float((total/Math::absf(total)) * speed);
	float* step = new float(total/(*addangle));

	t->data[0] = (void*)angle;
	t->data[1] = (void*)addangle;
	t->data[2] = (void*)step;

	setTaskInfo(t, L);

	tasks.add(t);

	return 0;
}

int invRTSGame::move(lua_State* L)
{
	float x = (float)lua_tonumber(L, 1);
	float y = (float)lua_tonumber(L, 2);
	float z = (float)lua_tonumber(L, 3);
	unsigned int id = lua_tonumber(L, 5);
	Vec3f* pos = &findObject(id)->pos;
	float speed = (float)lua_tonumber(L, 4);
	if(speed <= 0.f)
	{
		pos->x += x;
		pos->y += y;
		pos->z += z;
		return 0;
	}

	invTask* t = new invTask;
	t->type = invTask::MOVE;
	t->unitID = id;
	t->data[0] = (void*)pos;

	Vec3f* addpos = new Vec3f(x,y,z);
		
	float length = 1.f/addpos->getLengthInv();
	float* step = new float(length/speed);
	addpos->normalise();
	(*addpos) *= speed;

	t->data[1] = (void*)addpos;
	t->data[2] = (void*)step;

	setTaskInfo(t, L);
	tasks.addBack(t);

	return 0;
}

int invRTSGame::mouseclickdown(lua_State* L)
{
	int click = 0;
	// TODO : mouse-wheel support
	if(sinput->mousebutton == SDL_BUTTON_LEFT && sinput->mousestate == 1)
		click = 1;	// left-click
	else if(sinput->mousebutton == SDL_BUTTON_RIGHT && sinput->mousestate == 1)
		click = 2;	// right-click
	
	lua_pushnumber(L, click);
	return 1;
}

int invRTSGame::mouseclickup(lua_State* L)
{
	int click = 0;
	// TODO : mouse-wheel support
	if(sinput->mousebutton == SDL_BUTTON_LEFT && sinput->mousestate == 2)
		click = 1;	// left-click
	else if(sinput->mousebutton == SDL_BUTTON_RIGHT && sinput->mousestate == 2)
		click = 2;	// right-click
	lua_pushnumber(L, click);
	return 1;
}

ModelDef* invRTSGame::findModelDef(int id)
{
	mps.rewind();
	while(!mps.isEnd())
	{
		if(mps.current()->id == id)
			return mps.current();
		mps.next();
	}
	return NULL;
}

int invRTSGame::registerUnit(lua_State* L)
{
	int defid = lua_tonumber(L, 1);

	ModelDef* def = mps.getIndex(defid-1);
	invEntity* e = new invEntity(def);

	e->id = unitIDCounter++;
	e->id |= (ENTITY_IDH<<29);
	e->setMesh(def->mesh);
	e->tex = def->tex;

	entities.addBack(e);

	lua_pushnumber(L, e->id);
	return 1;
}

int invRTSGame::unregisterUnit(lua_State* L)
{
	invEntity* e = findEntity(lua_tonumber(L, 1));

	for(int i=0;i<nb_selectedl;i++)
	{
		if(selectedl[i] == e)
		{
			selectedl[i] = NULL;
			nb_selectedl--;
			break;
		}
	}

	entities.seek(e);
	entities.erase();

	return 0;
}

int invRTSGame::killTasks(lua_State* L)
{
	int unitID = lua_tonumber(L, 1);
	bool killAll = false;

	if(lua_gettop(L) == 2)
		killAll = lua_toboolean(L, 2);
	tasks.rewind();
	while(!tasks.isEnd())
	{
		if((tasks.current()->unitID == unitID || unitID == 0) &&
			(tasks.current()->background == false || killAll)) {
			tasks.erase();
		}
		else tasks.next();
	}

	return 0;
}

int invRTSGame::killTasksWithThread(lua_State* L)
{
	int threadID = lua_tonumber(L, 1);
	bool killAll = false;

	if(lua_gettop(L) == 2)
		killAll = lua_toboolean(L, 2);
	tasks.rewind();
	while(!tasks.isEnd())
	{
		if((tasks.current()->thread == threadID || threadID == 0) &&
			(tasks.current()->background == false || killAll)) {
			tasks.erase();
		}
		else tasks.next();
	}
	return 0;
}

int invRTSGame::moveToPoint(lua_State* L)
{
	invTask* t = new invTask;
	t->type = invTask::MOVING_TO_POINT;
	t->unitID = lua_tonumber(L, 3);
	t->e = findEntity(t->unitID);

	Vec3f* dest = new Vec3f(lua_tonumber(L, 1), 0.f, lua_tonumber(L, 2));
	t->data[0] = (void*)pfgrid;
	t->data[1] = (void*)dest;
	t->e->deletePath();
	t->e->path = searchPath(t->e->pos.x, t->e->pos.z, dest->x, dest->z, pfgrid);
	
	setTaskInfo(t, L);
	tasks.addBack(t);

	return 0;
}

int invRTSGame::moveToEntity(lua_State* L)
{
	invTask* t = new invTask;
	t->type = invTask::MOVING_TO_ENTITY;
	t->unitID = lua_tonumber(L, 2);
	t->e = findEntity(t->unitID);

	invEntity* target = findEntity(lua_tonumber(L, 1));
	Vec3f* dest = &target->pos;
	t->data[0] = (void*)pfgrid;
	t->data[1] = (void*)dest;
	t->data[2] = (void*)target;
	t->e->deletePath();
	t->e->path = searchPath(t->e->pos.x, t->e->pos.z, dest->x, dest->z, pfgrid);

	setTaskInfo(t, L);

	tasks.addBack(t);

	return 0;
}

int invRTSGame::occupArea(lua_State* L)
{
	invEntity* e = findEntity(lua_tonumber(L, 1));
	pfgrid->setRect(e->pos.x - e->mdef->bx, e->pos.z - e->mdef->bz, e->pos.x + e->mdef->bx, e->pos.z + e->mdef->bz, true);

	return 0;
}

int invRTSGame::getActiveSignal(lua_State* L)
{
	lua_getglobal(L, "activeSignal");
	int signal = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);
	return signal;
}


int invRTSGame::CreateQuad(lua_State* L)
{
	invQuad* q = new invQuad;
	if(lua_gettop(L) == 2)
		q->in2D = lua_toboolean(L, 2);
	else
		q->in2D = true;

	SDL_Surface* img = loadImage(s_vfs->texture(lua_tostring(L, 1)));

	if(q->in2D)
	{
		q->w = (float)img->w;
		q->h = (float)img->h;
	}

	else
	{
		q->w = (float)img->w/100.f;
		q->h = (float)img->h/100.f;
	}

	q->tex = cvtSDLSurfToGLTex(img);
	
	quads.addBack(q);
	lua_pushnumber(L, quads.count);
	return 1;
}

int invRTSGame::CreateTextQuad(lua_State* L)
{
	invQuad* q = new invQuad;
	if(lua_gettop(L) == 2)
		q->in2D = lua_toboolean(L, 2);
	else
		q->in2D = true;

	quads.addBack(q);
	lua_pushnumber(L, quads.count);
	return 1;

}

int invRTSGame::CreateQuadInstance(lua_State* L)
{
	invQuad* q = quads.getIndex(lua_tonumber(L, 1)-1);
	invQuadInstance* i = new invQuadInstance(q);
	i->id = quadInstanceIDCounter++;
	i->id |= (QUAD_INSTANCE_IDH<<29);
	i->visible = true;

	quadinstances.add(i);
	lua_pushnumber(L, i->id);
	lua_pushnumber(L, i->q->w);
	lua_pushnumber(L, i->q->h);

	lua_getglobal(L, "activeThread");
	bool notThread = lua_isnil(L, -1);
	lua_pop(L, 1);

	if(notThread) return 3;

	invTask* t = new invTask;
	t->type = invTask::QUAD;
	t->data[0] = (void*)i;
	t->data[1] = (void*)&quadinstances;
	setTaskInfo(t, L);

	tasks.addBack(t);

	return 3;
}

int invRTSGame::deleteQuadInstance(lua_State* L)
{
	invQuadInstance* qi = findQuadInstance(lua_tonumber(L, 1));
	quadinstances.rewind();
	while(!quadinstances.isEnd())
	{
		if(quadinstances.current()->id == qi->id)
		{
			quadinstances.erase();
			break;
		}

		quadinstances.next();
	}

	return 0;
}

int invRTSGame::SetQuadSize(lua_State* L)
{
	invQuad* q = quads.getIndex(lua_tonumber(L, 1)-1);

	q->w = lua_tonumber(L, 2);
	q->h = lua_tonumber(L, 3);

	return 0;
}

invQuadInstance* invRTSGame::findQuadInstance(unsigned int id)
{
	quadinstances.rewind();
	while(!quadinstances.isEnd())
	{
		if(quadinstances.current()->id == id) return quadinstances.current();
		quadinstances.next();
	}

	fprintf(stderr, "ERROR : quad instance not found\n");
	return NULL;
}

int invRTSGame::SelectPlace(lua_State* L)
{
	selection_defid = lua_tonumber(L, 1);
	ModelDef* mp = mps.getIndex(selection_defid-1);
	selection_patron = new invEntity(mp);
	selection_patron->setMesh(mp->mesh);
	selection_patron->tex = mp->tex;
	selection_unit = true;

	invTask* t = new invTask;
	t->type = invTask::WAIT_FOR_SELECTION;
	t->data[0] = (void*)&selection_unit;
	t->recall = true;

	setTaskInfo(t, L);

	tasks.addBack(t);

	SDL_ShowCursor(SDL_DISABLE);

	return 0;
}

int invRTSGame::getSelectedResult(lua_State* L)
{
	if(selection_success)
	{
		lua_pushboolean(L, 1);
		lua_pushnumber(L, selection_place.x);
		lua_pushnumber(L, selection_place.z);
	}
	
	else
	{
		lua_pushboolean(L, 0);
		lua_pushnil(L);
		lua_pushnil(L);
	}

	return 3;
}

int invRTSGame::SetAnim(lua_State* L)
{
	invEntity* e = findEntity(lua_tonumber(L, 3));
	e->StartFrame = lua_tonumber(L, 1);
	e->EndFrame = lua_tonumber(L, 2);

	if(e->CurFrame < e->StartFrame || e->CurFrame > e->EndFrame)
		e->CurFrame = e->StartFrame;

	return 0;
}

int invRTSGame::getPos(lua_State* L)
{
	Vec3f* pos = &findObject(lua_tonumber(L, 1))->pos;
	lua_pushnumber(L, pos->x);
	lua_pushnumber(L, pos->y);
	lua_pushnumber(L, pos->z);

	return 3;
}

int invRTSGame::setPos(lua_State* L)
{
	Vec3f* pos = &findObject(lua_tonumber(L, 4))->pos;
	pos->x = lua_tonumber(L, 1);
	pos->y = lua_tonumber(L, 2);
	pos->z = lua_tonumber(L, 3);

	return 0;
}

invMap* invRTSGame::getMap() const
{
	return drawgrid;
}

// TODO : put this somewhere else
int next_p2(int x) 
{
	int y = 1;
	while(y < x) y <<= 1;	
	return y;
}

void invRTSGame::generateFow()
{
	glGenTextures(1, &fow);
	glBindTexture(GL_TEXTURE_2D, fow);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void invRTSGame::updateUnitFow(invEntity* e)
{
	int sight = e->mdef->sight;
	int sight2 = sight * sight;
	int idx = pfgrid->getIndex(e->pos.x, e->pos.z);
	int x = idx%pfgrid->countx;
	int y = idx/pfgrid->county;

	for(int i=-sight;i<sight;i++)
	{
		for(int j=-sight;j<sight;j++)
		{
			int dist2 = (i*i + j*j);
			int rx = i + x, ry = j + y;
			if(dist2 < sight2 && pfgrid->inBound(rx, ry))
			{
				float percent = (float)(sight - sqrtf(dist2))/(float)sight;
				unsigned char new_alpha = (unsigned char)(percent*255.f);
				unsigned char already = pfgrid->visible[ry * pfgrid->countx + rx];
				if(already < new_alpha)
					pfgrid->visible[ry * pfgrid->countx + rx] = new_alpha;
			}
		}
	}

}

void invRTSGame::updateFow()
{
	for(int i=0;i<pfgrid->countx * pfgrid->county;i++)
	{
		Uint8 alpha = pfgrid->visible[i];
		if(alpha)
		{
			Uint8 exp = pfgrid->explored[i];
			if(alpha/3 > exp) {
				exp = alpha/3;
				pfgrid->explored[i] = exp;
			}
			pfgrid->visible[i] = exp;
		}

		else
		{
			pfgrid->visible[i] = 0;
		}
	}

	entities.rewind();
	while(!entities.isEnd())
	{
		updateUnitFow(entities.current());
		entities.next();
	}
}

void invRTSGame::computeFow()
{
	glBindTexture(GL_TEXTURE_2D, fow);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pfgrid->countx, pfgrid->county, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (void*)pfgrid->visible);
}

void invRTSGame::drawMap()
{
	if(draw_map){
		computeFow();
		drawgrid->draw(fow, true);
	}
}

int invRTSGame::loadMap(lua_State* L)
{
	SDL_Surface* map = loadImage(s_vfs->texture(lua_tostring(L, 1)));
	SDL_LockSurface(map);
	
	lua_pushlightuserdata(L, (void*)map);
	lua_pushnumber(L, map->w);
	lua_pushnumber(L, map->h);


	return 3;
}

int invRTSGame::getPixel(lua_State* L)
{
	SDL_Surface* map = (SDL_Surface*)lua_touserdata(L, 1);
	int x = lua_tonumber(L, 2);
	int y = lua_tonumber(L, 3);

	Uint8* pixel = (Uint8*)map->pixels;
	lua_pushnumber(L, pixel[y * map->pitch + x]);

	return 1;
}

int invRTSGame::unloadMap(lua_State* L)
{
	SDL_Surface* map = (SDL_Surface*)lua_touserdata(L, 1);

	SDL_UnlockSurface(map);
	SDL_FreeSurface(map);

	return 0;
}

int invRTSGame::createTerrain(lua_State* L)
{
	drawgrid = new invMap(lua_tonumber(L, 1), lua_tonumber(L, 2));
	pfgrid = new invPathFinderMap(lua_tonumber(L, 1)*16.0, lua_tonumber(L, 2)/16.0);
	generateFow();
	draw_map = true;
	return 0;
}

int invRTSGame::loadTerrainTextures(lua_State* L)
{
	drawgrid->loadTex(lua_tostring(L, 1), lua_tostring(L, 2));
	return 0;
}

int invRTSGame::destroyTerrain(lua_State* L)
{
	if(pfgrid) delete pfgrid;
	if(drawgrid) delete drawgrid;
	draw_map = false;

	return 0;
}

int invRTSGame::insertMQuad(lua_State* L)
{
	mquad[mquad_count++] = findQuadInstance(lua_tonumber(L, 1));
	return 0;
}

int invRTSGame::deleteMQuad(lua_State* L)
{
	invQuadInstance* target = findQuadInstance(lua_tonumber(L, 1));
	if(mquad_previous_in == target) mquad_previous_in = NULL;

	deleteMQuadP(target);
	return 0;
}

void invRTSGame::deleteMQuadP(invQuadInstance* target)
{
	for(int i=0;i<mquad_count;i++)
	{
		if(mquad[i] == target)
		{
			for(int j=i;j<mquad_count;j++)
				mquad[j] = mquad[j+1];
			break;
		}
	}
	mquad_count--;
}

int invRTSGame::changeOpacity(lua_State* L)
{
	invQuadInstance* qi = findQuadInstance(lua_tonumber(L, 3));
	float time = lua_tonumber(L, 2);
	float opacity = lua_tonumber(L, 1);
	if(time == 0.f)
	{
		qi->opacity = opacity;
		return 0;	
	}

	float* addop = new float((opacity - qi->opacity)/time);
	float* ptr_time = new float(time);
	
	invTask* t = new invTask;
	t->type = invTask::CHANGE_OPACITY;
	t->data[0] = (void*)qi;
	t->data[1] = (void*)addop;
	t->data[2] = (void*)ptr_time;
	t->recall = false;

	setTaskInfo(t, L);
	tasks.add(t);
	return 0;
}

int invRTSGame::resize(lua_State* L)
{
	invQuadInstance* qi = findQuadInstance(lua_tonumber(L, 3));
	qi->q->w = lua_tonumber(L, 1);
	qi->q->h = lua_tonumber(L, 2);
	
	return 0;
}

int invRTSGame::resetElapsedTime(lua_State* L)
{
	ignore_next_elapsedTime = true;
	return 0;
}

int invRTSGame::getResolution(lua_State* L)
{
	lua_pushnumber(L, window_width);
	lua_pushnumber(L, window_height);
	return 2;
}

void invRTSGame::drawMinimap()
{
	glDisable(GL_TEXTURE_2D);
	if(mm_visible)
	{
		glColor3f(0.f, 0.3f, 0.0f);
		glBegin(GL_QUADS);
		
		glVertex3f(mm_posx, mm_posy, -11.f);
		glVertex3f(mm_posx + mm_width, mm_posy, -11.f);
		glVertex3f(mm_posx + mm_width, mm_posy + mm_height, -11.f);
		glVertex3f(mm_posx, mm_posy + mm_height, -11.f);

		glEnd();

		glColor3f(1.f, 1.f, 1.f);
		glBegin(GL_POINTS);

		entities.rewind();
		while(!entities.isEnd())
		{
			invEntity* e = entities.current();
			if(e->mdef->selectable)
			{
				float posx = (e->pos.x - drawgrid->startx)/(pfgrid->countx * pfgrid->cellx) * mm_width;
				float posy = (e->pos.z - pfgrid->starty)/(pfgrid->county * pfgrid->celly) * mm_height;

				glVertex3f(mm_posx + posx, mm_posy + posy, -8.f);
			}

			entities.next();
		}

		glEnd();
	}
}

int invRTSGame::setQuadVisible(lua_State* L)
{
	invQuadInstance* qi = findQuadInstance(lua_tonumber(L, 2));
	qi->visible = lua_toboolean(L, 1);

	return 0;
}

int invRTSGame::getMouseRayPos(lua_State* L)
{
	lua_pushnumber(L, terrainX);
	lua_pushnumber(L, terrainZ);

	return 2;
}

int invRTSGame::loadSound(lua_State* L)
{
	const char* filename = lua_tostring(L, 1);
	int id = ssys.add(filename);
	lua_pushnumber(L, id);
	return 1;
}

int invRTSGame::playSound(lua_State* L)
{
	int id = lua_tonumber(L, 1);
	ssys.play(id);
	return 0;
}

int invRTSGame::setMinimap(lua_State* L)
{
	mm_visible = lua_toboolean(L, 1);
	return 0;
}

int invRTSGame::setClearColor(lua_State* L)
{
	glClearColor(lua_tonumber(L, 1),
				   lua_tonumber(L, 2),
				   lua_tonumber(L, 3), 1.f);

	return 0;
}

int invRTSGame::loadFont(lua_State* L)
{
	Font* font = flib.loadFont(lua_tostring(L, 1), lua_tonumber(L, 2));
	fonts.addBack(font);
	lua_pushnumber(L, fonts.count);

	return 1;
}

int invRTSGame::setText(lua_State* L)
{
	invTextBuffer* tb = findTextBuffer(lua_tonumber(L, 2));
	tb->setText(lua_tostring(L, 1));

	int size = tb->font->getSize(lua_tostring(L, 1));
	lua_pushnumber(L, size);
	lua_pushnumber(L, tb->font->size);

	return 2;
}

int invRTSGame::CreateTextBuffer(lua_State* L)
{
	invTextBuffer* tb = new invTextBuffer;
	tb->setFont(fonts.getIndex(lua_tonumber(L, 1) - 1));
	tb->id = textBufferIDCounter++;
	tb->id |= (TEXT_BUFFER_IDH<<29);
	tb->visible = true;

	textbuffers.add(tb);
	lua_pushnumber(L, tb->id);

	lua_getglobal(L, "activeThread");
	bool notThread = lua_isnil(L, -1);
	lua_pop(L, 1);

	if(notThread) return 1;
	
	invTask* t = new invTask;
	t->type = invTask::TEXTBUFFER;
	t->data[0] = (void*)tb;
	t->data[1] = (void*)&textbuffers;
	setTaskInfo(t, L);

	tasks.addBack(t);

	return 1;
}

invTextBuffer* invRTSGame::findTextBuffer(unsigned int id)
{
	textbuffers.rewind();
	while(!textbuffers.isEnd())
	{
		if(textbuffers.current()->id == id)
			return textbuffers.current();
		textbuffers.next();
	}

	fprintf(stderr, "ERROR : no text buffers found\n");
	return NULL;
}

int invRTSGame::DeleteTextBuffer(lua_State* L)
{
	invTextBuffer* tb = findTextBuffer(lua_tonumber(L, 1));
	if(tb)
	{
		textbuffers.rewind();
		while(!textbuffers.isEnd())
		{
			if(textbuffers.current()->id == tb->id)
			{
				textbuffers.erase();
				break;
			}

			textbuffers.next();
		}
	}
	return 0;
}
