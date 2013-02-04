info = {
	name = "collectorUnits",
	group = "UnitCap",
	depedency = "UnitCreation SearchDeposit",
	entry_point = "Init",
}

local function collecting_process(resource_unit, unit, resource_type)
	while true do
		unit.food = 0
		unit.wood = 0
		unit.stone = 0
		unit.gold = 0
		SendSignal(69, unit.id, resource_unit.id)
		WaitForMove(unit.id)
		Sleep(0.1)
		if resource_type == 1 then
			SetAnim(unit.frames_cultiv[1], unit.frames_cultiv[2], unit.id)
			while unit.food < 100 do
				unit.food = unit.food + 10
				Sleep(0.5)
			end
		elseif resource_type == 2 then
			SetAnim(unit.frames_chop[1], unit.frames_chop[2], unit.id)
			while unit.wood < 100 do
				unit.wood = unit.wood + 10
				SendSignal(20, resource_unit.id)
				Sleep(0.5)
			end
		elseif resource_type == 3 then
			SetAnim(unit.frames_mine[1], unit.frames_mine[2], unit.id)
			while unit.gold < 100 do
				unit.gold = unit.gold + 10
				Sleep(0.5)
			end
		elseif resource_type == 4 then
			SetAnim(unit.frames_mine[1], unit.frames_mine[2], unit.id)
			while unit.stone < 100 do
				unit.stone = unit.stone + 10
				Sleep(0.5)
			end
		else
			error("unknown type of resource")
		end

		
		SetAnim(unit.frames_walk[1], unit.frames_walk[2], unit.id)
		local deposit = searchDeposit(unit.id)
		if not deposit then break end

		SendSignal(69, unit.id, deposit.id)
		WaitForMove(unit.id)
		Sleep(0.1)
	
		resource[1] = resource[1] + unit.food
		resource[2] = resource[2] + unit.wood
		resource[3] = resource[3] + unit.gold
		resource[4] = resource[4] + unit.stone
		updateBarResources()
	end
end

local function interact_with(unit, selid)
	local u = units[selid]
	if u.resource then
		StartThread(collecting_process, false, u, unit, u.resource)
	end
end

local function prepare_units()
	for _,unitDef in pairs(unitDefs) do
		if unitDef.canCollect then
			unitDef:attachToUnit(interact_with, 72)
		end
	end
end

function Init()
	AttachGlobal(prepare_units, 55)
end
