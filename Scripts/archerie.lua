info = {
	name = "Archerie",
	entry_point = "Start",
}

local prop = {
	name = "archerie",
	meshfile = "archerie.imf",
	texfile = "archerie.png",
	alpha = 0,

	boundingx = 2.5,
	boundingz = 2.5,
	scale = 1.5,

	selectable = true,
	
	builds = {"archer", "arbaletrier"},

	food_cost = 0,
	wood_cost = 100,
	gold_cost = 50,
	stone_cost = 0,

	max_health = 100,

}

function Start()
	local udef = UnitDef:createUnitDef(prop, {})
end
