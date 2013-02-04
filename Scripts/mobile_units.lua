info = {
	name = "MobileUnits",
	entry_point = "Init",
}

local function walking_process(unit)
	SetAnim(unit.frames_walk[1], unit.frames_walk[2], unit.id)
	WaitForMove(unit.id)
	SetAnim(unit.frames_stand[1], unit.frames_stand[2], unit.id)
end

local function walkToPoint(unit, x, z)
	MoveToPoint(x, z, unit.id)
	StartThread(walking_process, false, unit)
end

local function walkToEntity(unit, targetid)
	MoveToEntity(targetid, unit.id)
	StartThread(walking_process, false, unit)
end

local function prepare_units()
	for _,unitDef in pairs(unitDefs) do
		if unitDef.canMove then
			unitDef:attachToUnit(walkToPoint, 70)
			unitDef:attachToUnit(walkToEntity, 69)
		end
	end
end

function Init()
	AttachGlobal(prepare_units, 55)
end
