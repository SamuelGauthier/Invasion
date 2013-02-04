info = {
	name = "Mine_pierre",
	group = "UnitCreation",
	depedency = "Base",
	entry_point = "Start",
}

local prop = {
	name = "mine_pierre",
	meshfile = "mine_pierre.imf",
	texfile = "mine_pierre.png",
	alpha = 0,

	boundingx = 0.9,
	boundingz = 0.9,
	scale = 1,

	selectable = true,
	resource = 4,
}

function Start()
	local udef = UnitDef:createUnitDef(prop, {})
end
