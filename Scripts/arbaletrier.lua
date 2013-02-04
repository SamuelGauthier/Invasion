info = {
	name = "Arbaletrier",
	entry_point = "Start",
}

local prop = {
	name = "arbaletrier",
	meshfile = "arbaletrier.imf",
	texfile = "arbaletrier.png",
	alpha = 1,

	boundingx = 0.2,
	boundingz = 0.2,
	scale = 0.1,

	selectable = true,
	canMove = true,
	canAttack = true,

	frames_walk = {64, 79},
	frames_stand = {0, 63},

	max_health = 100,

	food_cost = 0,
	wood_cost = 0,
	gold_cost = 0,
	stone_cost = 0,
}

function Start()
	local udef = UnitDef:createUnitDef(prop, {})
end
