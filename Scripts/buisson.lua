info = {
	name = "Buisson",
	entry_point = "Start",
}

local prop = {
	name = "buisson",
	meshfile = "buisson.imf",
	texfile = "buisson.png",
	alpha = 0,

	boundingx = 0.9,
	boundingz = 0.9,
	scale = 1,

	selectable = true,
	resource = 1,
}

function Start()
	local udef = UnitDef:createUnitDef(prop, {})
end
