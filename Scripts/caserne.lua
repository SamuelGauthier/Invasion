info = {
	name = "Caserne",
	entry_point = "Start",
}

local prop = {
	name = "caserne",
	meshfile = "caserne.imf",
	texfile = "caserne.png",
	alpha = 0,

	boundingx = 3.5,
	boundingz = 3.5,
	scale = 1,

	selectable = true,
	
	builds = {"archer"},

	food_cost = 0,
	wood_cost = 100,
	gold_cost = 50,
	stone_cost = 0,

	max_health = 1400,

}

function Start()
	local udef = UnitDef:createUnitDef(prop, {})
end
