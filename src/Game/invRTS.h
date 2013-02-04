#ifndef INVRTS_H
#define INVRTS_H
typedef unsigned char Uint8;

#include <lua.hpp>
#include <GL/glew.h>
#include <SDL/SDL_ttf.h>
#include "invEntity.h"
#include "invSchedule.h"
#include "invTalk.h"
#include "pathfinding.h"
#include "invInput.h"
#include "invVFS.h"
#include "invScript.h"
#include "invSound.h"
#include "font.h"
#include <dirent.h>


enum ID_HEADER
{
	ENTITY_IDH = (1<<0),
	QUAD_INSTANCE_IDH = (1<<1),
	TEXT_BUFFER_IDH = (1<<2),
};

//------------------------------------------------
// invRTSGame
//------------------------------------------------
class invRTSGame
{
	void beginFX(int alpha, float opacity);
	void endFX(int alpha);
public:
	
	invRTSGame();
	~invRTSGame();

	void move(float ElapsedTime);
	void drawEntities();
	void drawQuads();
	void draw2D();
	void selection(int height);
	void terrainPoint();
	void command();
	void setWindowResolution(int w, int h);
	void initTerrainDim(int count, float size);
	invEntity* spawn(const char* name, Vec3f* pos = NULL);
	invEntity* spawn(const char* name, invEntity* e);
	static invObject* findObject(unsigned int id);
	static invEntity* findEntity(unsigned int id);
	static invEntity* findEntity(const char* name);
	static invEntity* spawnChildren(ModelDef* mp);
	static ModelDef* findModelDef(int id);
	static invEntity* findChildren(invEntity* parent, int* offset);
	static invQuadInstance* findQuadInstance(unsigned int id);
	static void insertQuadInstance(invQuadInstance* qi);
	static int deleteQuadInstance(lua_State* L);
	
	// lua
	static void setTaskInfo(invTask* t, lua_State* L);
	void initScriptEngine();
	void runScripts();
	void processTasks(float elapsedTime);
	static int sleep(lua_State* L);
	static int turn(lua_State* L);
	static int changeOpacity(lua_State* L);
	static int resize(lua_State* L);

	static int waitforturn(lua_State* L);
	static int move(lua_State* L);
	static int mouseclickdown(lua_State* L);
	static int mouseclickup(lua_State* L);
	static int registerUnit(lua_State* L);
	static int unregisterUnit(lua_State* L);
	static int getModelDefID(lua_State* L);
	static int moveToPoint(lua_State* L);
	static int moveToEntity(lua_State* L);

	static int killTasks(lua_State* L);
	static int killTasksWithThread(lua_State* L);
	static int getActiveSignal(lua_State* L);
	static int waitForMove(lua_State* L);
	static int SetAnim(lua_State* L);
	static int getPos(lua_State* L);
	static int updateQuadZ(lua_State* L);

	static int loadFont(lua_State* L);
	static int CreateQuad(lua_State* L);
	static int CreateTextQuad(lua_State* L);
	static int CreateQuadInstance(lua_State* L);
	static int SetQuadSize(lua_State* L);
	static int SelectPlace(lua_State* L);

	static int getSelectedType(lua_State* L);
	static int getSelectedResult(lua_State* L);
	static int CreateUnitDef(lua_State* L);

	static int createTerrain(lua_State* L);
	static int loadTerrainTextures(lua_State* L);
	static int destroyTerrain(lua_State* L);

	static int loadMap(lua_State* L);
	static int getPixel(lua_State* L);
	static int unloadMap(lua_State* L);

	static int resetElapsedTime(lua_State* L);
	static int getResolution(lua_State* L);
	static int setPos(lua_State* L);
	static int setQuadVisible(lua_State* L);

	static int getMouseRayPos(lua_State* L);

	static int setMinimap(lua_State* L);
	static int setClearColor(lua_State* L);
	static int myprint(lua_State* L);
	static int waitForKey(lua_State* L);
	static int togglePause(lua_State* L);

	static int togglePicking(lua_State* L);

	// quad monitor
	void quad_monitoring();
	static int insertMQuad(lua_State* L);
	static int deleteMQuad(lua_State* L);
	static void deleteMQuadP(invQuadInstance* q);

	static int setText(lua_State* L);
	static int CreateTextBuffer(lua_State* L);
	static invTextBuffer* findTextBuffer(unsigned int id);
	static int DeleteTextBuffer(lua_State* L);

	// sound
	static int loadSound(lua_State* L);
	static int playSound(lua_State* L);

	Vec2f getGridArea(Vec2f pos);
	bool isFreeGridArea(Vec2f pos, Vec2f size);
	void fillGridArea(Vec2f pos, Vec2f size);
	static int occupArea(lua_State* L);
	void loadMap(const char* filename);
	static void computeFow();
	static void generateFow();
	static void updateFow();
	static void updateUnitFow(invEntity* e);

	void drawMinimap();

	lua_State* mainL;
	lua_State* guiL;

	int resource[4];

	int pickingX, pickingY;
	
	bool terrain_point;
	GLuint circle; 			// selection circle

	bool areaSel;
	float startAreaSelX, startAreaSelZ;
	float endAreaSelX, endAreaSelZ;

	int startMouseSelX, startMouseSelY;
	int endMouseSelX, endMouseSelY;
	int* gui_listener;
	bool gui_passed;
	bool* gui_focused;

	invMap* getMap() const;
	void drawMap();
private:
	
	void selectEntityInArea();
	int id_counter;
};



#endif
