info = {
	name = "Units",
	group = "Units",
	depedency = "",
	entry_point = "",
}

-- Units
units = {}
unitDefs = {}
UnitDef = {}

nb_selected = 0
selected_units = {}

target_unit = nil
----------------------------------------------------------------------
-- Units
----------------------------------------------------------------------
function UnitDef:createUnitDef(properties, unused)
	local unitDef = {}
	local unitDefID = RegisterUnitDef(properties)
	unitDef.id = unitDefID
	unitDef.funs = {}
	unitDefs[unitDefID] = unitDef
	setmetatable(properties, {__index = self})
	setmetatable(unitDef, {__index = properties})

	return unitDef
end

function UnitDef:attachToUnit(fun, sig)
	self.funs[sig] = fun
end

function UnitDef:createUnit()
	local unit = {}
	setmetatable(unit, {__index = self})
	local unitID = RegisterUnit(self.id)

	unit.id = unitID
	unit.defid = self.id
	units[unitID] = unit

	SendSignal(101, 0, unit)
	
	return unit
end

function getUnitDef(name)
	for i=1,#unitDefs do
		if unitDefs[i].name == name then return unitDefs[i] end
	end
end

function DestroyUnit(unit)
	UnregisterUnit(unit.id)
end
