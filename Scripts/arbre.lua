info = {
	name = "Arbre",
	entry_point = "Start",
}

local prop = {
	name = "arbre",
	meshfile = "arbre.imf",
	texfile = "arbre.png",
	alpha = 2,

	boundingx = 0.3,
	boundingz = 0.3,
	scale = 0.3,

	selectable = true,
	resource = 1,
}

function Start()
	local udef = UnitDef:createUnitDef(prop, {})
end
