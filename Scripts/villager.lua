info = {
	name = "Villager",
	entry_point = "Start",
}

local prop = {
	name = "villager",
	meshfile = "villager-fermale.imf",
	texfile = "villager-fermale.png",
	alpha = 1,

	boundingx = 0.2,
	boundingz = 0.2,
	scale = 0.1,

	selectable = true,
	canMove = true,
	canCollect = true,

	builds = {"moulin", "caserne", "tour_archer", "archerie"},

	frames_walk = {30, 41},
	frames_stand = {20, 29},
	frames_build = {80, 90},
	frames_mine = {80, 90},

	max_health = 100,

	food_cost = 80,
	wood_cost = 0,
	gold_cost = 0,
	stone_cost = 0,

	construction_speed = 10,

	defense = 1,
}

local param = {
	food = 0,
	wood = 0,
	gold = 0,
	stone = 0,
}

function Start()
	local udef = UnitDef:createUnitDef(prop, param)
end
