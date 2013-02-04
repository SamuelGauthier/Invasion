info = {
	name = "ProduceUnit",
	entry_point = "Init",
}

local function add_to_queue(unit, newUnitDef)
	if not unit.queue then 
		unit.queue = {} 
		unit.queue_count = 0
	end
	unit.queue[unit.queue_count + 1] = newUnitDef
	unit.queue_count = unit.queue_count + 1
end

local function next_to_queue(unit)
	for i=1,unit.queue_count do
		unit.queue[i] = unit.queue[i+1]
	end
	unit.queue_count = unit.queue_count - 1
end

local function produce_unit_process(unit)
	unit.producing = true
	while unit.queue_count > 0 do
		Sleep(5)
		local newUnitDef = unit.queue[1]
		local x,y,z = GetPos(unit.id)
		local newUnit = newUnitDef:createUnit()
		SetPos(x + unit.boundingx, 0, z + unit.boundingz, newUnit.id)
		SendSignal(70, newUnit.id, x + unit.boundingx + 3, z + unit.boundingz + 3)

		next_to_queue(unit)
	end
	unit.producing = nil 
end

local function produce_unit(unit, newUnitDef)
	if buy(newUnitDef) and not unit.in_construction then
		add_to_queue(unit, newUnitDef)
		if not unit.producing then
			StartThread(produce_unit_process, true, unit)
		end
	end
end

local function prepare_units()
	for _,unitDef in pairs(unitDefs) do
		if unitDef.builds then
			unitDef:attachToUnit(produce_unit, 104)
		end
	end
end

function Init()
	AttachGlobal(prepare_units, 55)
end
