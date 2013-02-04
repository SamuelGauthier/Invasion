info = {
	name = "GameLauncher",
	entry_point = "Init",
}

local font18 = LoadFont("poor-richard-2.ttf", 18)

local function start_game_process()
	SetMinimap(true)
	InitMsgBox("msgbox.bmp", font18)
	
	SendSignal(6) -- transition_begin
	SendSignal(9) -- load_terrain
	SendSignal(10) -- interface_init
	SendSignal(16)
	
	SendSignal(7) -- transition_loop
	Sleep(2)
	MsgBox("Welcome to Invasion")
	MsgBox("Survive 4 waves")
	SendSignal(100) -- next_wave
	
	TogglePicking()
end

local function start_game()
	StartThread(start_game_process, false)
end


function Init()
	AttachGlobal(start_game, 1)
end
