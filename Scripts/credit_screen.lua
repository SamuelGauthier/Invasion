info = {
	name = "CreditScreen",
	entry_point = "Init",
}

local mask_m
local mask
local logo_m
local logo

local function credit_screen_process()
	width, height = GetResolution()
	
	mask_m = CreateQuad("black.bmp")
	mask = CreateQuadInstance(mask_m)
	Resize(width, height, mask)
	Move(0, 0, -2, 0, mask)

	logo_m = CreateQuad("Logo_invasion_team.png")
	logo = CreateQuadInstance(logo_m)
	Resize(width, height, logo)
	Move(0, 0, -3, 0, logo)

	Sleep(1)
	ChangeOpacity(0, 1.5, mask)
	Sleep(8)

	ChangeOpacity(1, 1.5, mask)
	Sleep(2)

	SendSignal(2) -- menu_init
end

local function credit_screen()
	SetClearColor(1, 1, 1)
	SetMinimap(false)
	print("credit_screen_process")
	StartThread(credit_screen_process, false)
end

function Init()
	AttachGlobal(credit_screen, 3)
end
