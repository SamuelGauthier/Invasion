info = {
	name = "Archer",
	entry_point = "Start",
}

local prop = {
	name = "archer",
	meshfile = "archer.imf",
	texfile = "archer.png",
	alpha = 1,

	boundingx = 0.2,
	boundingz = 0.2,
	scale = 0.1,

	selectable = true,
	canMove = true,
	canAttack = true,

	frames_walk = {50, 60},
	frames_stand = {0, 49},

	max_health = 100,

	food_cost = 0,
	wood_cost = 0,
	gold_cost = 0,
	stone_cost = 0,
}

function Start()
	local udef = UnitDef:createUnitDef(prop, {})
end
