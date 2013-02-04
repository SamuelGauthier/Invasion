info = {
	name = "BarbarianWaves",
	entry_point = "Init",
}

local spawn = {x = 40, z = 40}

barbarian_count = 0
wave_count = 3
local function spawn_barbarian()
	local barbarian_def = getUnitDef("barbarian")
	local barbarian = barbarian_def:createUnit()
	SetPos(spawn.x, 0.001, spawn.z, barbarian.id)
	SendSignal(102, barbarian.id)
end

local function wave_process()
	for i=1,4 do
		spawn_barbarian()
		Sleep(2)
	end
	barbarian_count = barbarian_count + 4
end

local function start_wave()
	StartThread(wave_process, false)
end

local function next_wave()
	if wave_count > 0 then
		SendSignal(67)
		wave_count = wave_count - 1
	else
		SendSignal(77) -- finish, winned
	end
end

local function barbarian_killed(unit)
	if unit.enemy then
		barbarian_count = barbarian_count - 1
		if barbarian_count == 0 then
			SendSignal(100)
		end
	end
end

function Init()
	AttachGlobal(start_wave, 99)
	AttachGlobal(next_wave, 100)
	AttachGlobal(barbarian_killed, 103)
end