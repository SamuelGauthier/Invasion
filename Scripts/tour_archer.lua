info = {
	name = "TourArcher",
	entry_point = "Start",
}

local prop = {
	name = "tour_archer",
	meshfile = "tour_archer_age1.imf",
	texfile = "tour_archer_age1.png",
	alpha = 0,

	boundingx = 1.0,
	boundingz = 1.0,
	scale = 1,

	selectable = true,

	food_cost = 0,
	wood_cost = 100,
	gold_cost = 0,
	stone_cost = 50,

	max_health = 500,
	defense = 1,
}

function Start()
	local udef = UnitDef:createUnitDef(prop, {})
end
