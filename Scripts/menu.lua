info = {
	name = "Menu",
	entry_point = "Init",
}

local menu_panel
local play_button 
local options_button
local quit_button
font22 = LoadFont("poor-richard-2.ttf", 22)
local button_theme = {released = CreateQuad("buttonmenu.tif"), pressed = CreateQuad("buttonmenupressed.tif"), over = CreateQuad("buttonmenuselected.tif")}

local play_button_text
local options_button_text
local quit_button_text
local background
local bande
local logo_team

local function menu_init()
	local background_q = CreateQuad("Background_4_3.png")
	local bande_q = CreateQuad("bande_gauche_4_3.tif")
	local logo_team_q = CreateQuad("bande_gauche_titre.tif")

	local width, height = GetResolution()
	SetMinimap(false)
	SetClearColor(0, 0, 0)

	Interface:setTheme(Button, button_theme)

 	
	
	local bande_w, bande_h
	bande, bande_w, bande_h = CreateQuadInstance(bande_q)
	Move(40, 0, -5, 0, bande)
	bande_w = bande_w * (height/bande_h)
	Resize(bande_w, height, bande)
	
	local logo_team_w, logo_team_h
	logo_team, logo_team_w, logo_team_h = CreateQuadInstance(logo_team_q)
	Resize(bande_w, logo_team_h *(bande_w/logo_team_w), logo_team)
	Move(40, 20, -4, 0, logo_team)
	UpdateQuadZ(logo_team)
	
	-- button #1
	play_button = Interface:create(Button)
	play_button:move(40 + (bande_w-play_button.sizex)/2, 200)
	play_button:released()

	play_button_text = CreateTextBuffer(font22)
	local pbx, pby = SetText("Play", play_button_text)

	play_button:putText(play_button_text, pbx, pby)
	play_button:setSignal(12)
	
	-- button #2
	options_button = Interface:create(Button)
	options_button:move(40 + (bande_w-options_button.sizex)/2, 270)
	options_button:released()

	options_button_text = CreateTextBuffer(font22)
	local obx, oby = SetText("Options", options_button_text)

	options_button:putText(options_button_text, obx, oby)
	options_button:setSignal(1000)
	
	-- button #3
	quit_button = Interface:create(Button)
	quit_button:move(40 + (bande_w-quit_button.sizex)/2, height - 80)
	quit_button:released()

	quit_button_text = CreateTextBuffer(font22)
	local qbx, qby = SetText("Quit", quit_button_text)

	quit_button:putText(quit_button_text, qbx, qby)
	quit_button:setSignal(1000)
	
	-- Background
	background = CreateQuadInstance(background_q)
	Move(0, 0, -6, 0, background)
	Resize(width, height, background)
end

local function menu_deinit()
	Interface:destroy(play_button)
	DeleteTextBuffer(play_button_text)
	Interface:destroy(options_button)
	DeleteTextBuffer(options_button_text)
	Interface:destroy(quit_button)
	DeleteTextBuffer(quit_button_text)
	
	DeleteQuadInstance(background)
	DeleteQuadInstance(bande)
	DeleteQuadInstance(logo_team)
	
	SendSignal(3)
end

function Init()
	AttachGlobal(menu_init, 2)
	AttachGlobal(menu_deinit, 12)
end
