info = {
	name = "Moulin",
	group = "UnitCreation",
	depedency = "Base",
	entry_point = "Start",
}

local prop = {
	name = "moulin",
	meshfile = "moulin.imf",
	texfile = "moulin.png",
	alpha = 1,

	boundingx = 2.5,
	boundingz = 2.5,
	scale = 1,

	selectable = true,

	food_cost = 0,
	wood_cost = 50,
	gold_cost = 50,
	stone_cost = 0,
	
	max_health = 1000,
}

function Start()
	local udef = UnitDef:createUnitDef(prop, {})
end
