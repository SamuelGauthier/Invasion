info = {
	name = "Barbarian",
	entry_point = "Start",
}

local prop = {
	name = "barbarian",
	meshfile = "barbarian.imf",
	texfile = "barbarian.png",
	alpha = 1,

	boundingx = 0.2,
	boundingz = 0.2,
	scale = 0.1,

	selectable = true,
	canMove = true,

	ennemy = true,

	frames_walk = {90, 109},
	frames_stand = {0, 89},

	frames_attack = {110, 128},

	max_health = 100,
	attack = 10,
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
