info = {
	name = "Maison",
	group = "UnitCreation",
	depedency = "Base",
	entry_point = "Start",
}

local prop = {
	name = "maison",
	meshfile = "maison_age1.imf",
	texfile = "maison_age1.png",
	alpha = 1,

	boundingx = 2.5,
	boundingz = 2.5,
	scale = 1,

	selectable = true,
	deposit = true,
}

function Start()
	local udef = UnitDef:createUnitDef(prop, {})
end
