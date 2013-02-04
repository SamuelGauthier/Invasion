info = {
	name = "AttackUnits",
	entry_point = "Init",
}

local min_dist = 1

-- TODO optimize this 
local function attack_entity_process(unit, target)
	unit.attacking = true

	-- !repetition!
	SendSignal(69, unit.id, target.id)
	WaitForMove(unit.id)
	while target.health > 0 do
		local x1,_,z1 = GetPos(unit.id)
		local x2,_,z2 = GetPos(target.id)
		local dx = x2 - x1
		local dz = z2 - z1
		if ((dx * dx) + (dz * dz)) > (min_dist * min_dist) and target.canMove then -- optimize for static units
			SendSignal(69, unit.id, target.id)
			WaitForMove(unit.id)
		else
			SetAnim(unit.frames_attack[1], unit.frames_attack[2], unit.id)
			Sleep(1)
			target.health = target.health - unit.attack/target.defense
			local udef = getUnitDef("villager")
			print("real : " .. udef.id .. " saved : " .. target.defid)
			print(udef.health)
			UpdateUnitDisplayInfo()
		end
	end

	SendSignal(103, 0, target)

	SetAnim(unit.frames_stand[1], unit.frames_stand[2], unit.id)

	unit.attacking = false
end

local function attack_entity(unit, target)
	if unit.attack and target.defense then
		StartThread(attack_entity_process, false, unit, target)
	end
end

local function prepare_units()
	for _,unitDef in pairs(unitDefs) do
		if unitDef.attack then
			unitDef:attachToUnit(attack_entity, 80)
		end
	end
end

function Init()
	AttachGlobal(prepare_units, 55)
end
