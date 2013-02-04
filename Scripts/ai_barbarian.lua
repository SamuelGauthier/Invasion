info = {
	name = "AIBarbarian",
	entry_point = "Init",
}

local min_dist = 10

local function sweep(barbarian)
	for _,unit in pairs(units) do
		if not (unit == barbarian) then
			local unitx,_,unitz = GetPos(unit.id)
			local barbx,_,barbz = GetPos(barbarian.id)

			local dist_2 = (unitx - barbx) * (unitx - barbx) + (unitz - barbz) * (unitz - barbz)

			if dist_2 < (min_dist*min_dist) and unit.max_health and unit.defense then
				SendSignal(80, barbarian.id, unit)
				break
			end
		end
	end
end

local function sweeper(barbarian)
	while true do
		if not barbarian.attacking then
			SendSignal(70, barbarian.id, 0, 0)
			sweep(barbarian)
		end
		Sleep(5)
	end
end

local function sweep_init(barbarian)
	if barbarian.attack then
		StartThread(sweeper, true, barbarian)
	end
end

local function prepare_units()
	for _,unitDef in pairs(unitDefs) do
		if unitDef.canMove then
			unitDef:attachToUnit(sweep_init, 102)
		end
	end
end


function Init()
	AttachGlobal(prepare_units, 55)
end
