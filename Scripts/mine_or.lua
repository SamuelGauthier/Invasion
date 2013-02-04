info = {
	name = "Mine_or",
	entry_point = "Start",
}

local prop = {
	name = "mine_or",
	meshfile = "mine_or.imf",
	texfile = "mine_or.png",
	alpha = 0,

	boundingx = 0.9,
	boundingz = 0.9,
	scale = 1,

	selectable = true,
	resource = 3,
}

function Start()
	local udef = UnitDef:createUnitDef(prop, {})
end
