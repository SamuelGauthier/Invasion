info = {
	name = "builderUnits",
	group = "UnitCap",
	depedency = "UnitCreation",
	entry_point = "Start",
}

local function construction_place_process(unit, newUnitDef)
	if unit.construction_speed then
		SelectPlace(newUnitDef.id)
		local good, x, z = GetSelectedResult()
		if good and buy(newUnitDef) then
			local newUnit = newUnitDef:createUnit()
			newUnit.health = 0
			newUnit.in_construction = true
			SetPos(x, -10, z, newUnit.id)
			OccupArea(newUnit.id)

			SendSignal(71, unit.id, newUnit)
		end
	end
end

local function construction_place(unit, newUnitDef)
	StartThread(construction_place_process, false, unit, newUnitDef)
end

local function construction_process(unit, newUnit)
	local x, _, z = GetPos(newUnit.id)
	SendSignal(69, unit.id, newUnit.id)
	WaitForMove(unit.id)

	SetAnim(unit.frames_build[1], unit.frames_build[2], unit.id)

	while true do
		Sleep(0.5)
		if newUnit.health < newUnit.max_health then
			newUnit.health = newUnit.health + unit.construction_speed
			if newUnit.health > newUnit.max_health then
				newUnit.health = newUnit.max_health
			end
			UpdateUnitDisplayInfo()
			if newUnit.health == newUnit.max_health then
				SetPos(x, 0, z, newUnit.id)
				newUnit.in_construction = nil
				break
			end

		else 
			break end
	end

	SetAnim(unit.frames_stand[1], unit.frames_stand[2], unit.id)
end

local function construction(unit, newUnitDef, x, z)
	StartThread(construction_process, false, unit, newUnitDef, x, z)
end

local function prepare_units()
	for _,unitDef in pairs(unitDefs) do
		if unitDef.construction_speed then
			unitDef:attachToUnit(construction, 71)
		end
	end
end

function Start()
	AttachGlobal(construction_place, 73)
	AttachGlobal(prepare_units, 55)
end
