info = {
	name = "UnitValues",
	entry_point = "Init",
}

local function init_unit_values(unit)
	if unit.max_health then 
		unit.health = unit.max_health
	end
end

function Init()
	AttachGlobal(init_unit_values, 101)
end
