info = {
	name = "CentreVille",
	entry_point = "Start",
}

local prop = {
	name = "centre_ville",
	meshfile = "centre_ville_age1.imf",
	texfile = "centre_ville.png",
	alpha = 1,

	boundingx = 3.5,
	boundingz = 3.5,
	scale = 1,

	selectable = true,
	builds = {"villager"},
	deposit = true,

	max_health = 1500,
	defense = 2,
	sight = 30,
}

function Start()
	local udef = UnitDef:createUnitDef(prop, {})
end
