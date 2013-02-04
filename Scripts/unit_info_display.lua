info = {
	name = "UnitInfoDispay",
	entry_point = "",
}

font12 = LoadFont("poor-richard-2.ttf", 12)
local current_unit
local name_field 
local ipanel
local progressbar_theme = {frame = CreateQuad("statusbar.png"), bar = CreateQuad("green.png")}
local health_bar
local health_text
local attack_icon
local attack_text
local defense_icon
local defense_text

function ZeroUnitInfoDisplay()
	current_unit = nil
	SetText("", name_field)
	health_bar:hide()
	SetText("", health_text)
	SetQuadVisible(false, attack_icon)
	SetText("", attack_text)
	SetQuadVisible(false, defense_icon)
	SetText("", defense_text)
end

function InitUnitInfoDisplay(panel)
	ipanel = panel

	name_field = CreateTextBuffer(font22)
	health_text = CreateTextBuffer(font12)
	attack_text = CreateTextBuffer(font12)
	attack_icon = LoadTex("attack.png")
	defense_text = CreateTextBuffer(font12)
 	defense_icon = LoadTex("defense.png")

	Interface:setTheme(ProgressBar, progressbar_theme)
	health_bar = Interface:create(ProgressBar)

	ZeroUnitInfoDisplay()
end

local function SetUnitInfoDisplay(unit)
	current_unit = unit
	local spacer = 30
	local curx = ipanel.posx + spacer
	local cury = ipanel.posy + spacer
	local centerx = ipanel.posx + ipanel.sizex/2

	local name_fieldx, fontHeight = SetText(unit.name, name_field)
	SetPos(centerx - name_fieldx/2, cury, 0, name_field)
	cury = cury + fontHeight + spacer

	if unit.health then
		local h = unit.health
		local maxh = unit.max_health
		health_bar:show()
		health_bar:setValue(h/maxh)
		health_bar:move(centerx - health_bar.sizex/2, cury)

		cury = cury + health_bar.sizey + spacer/2

		local health_textx, fontHeight = SetText(h .. "/" .. maxh, health_text)
		SetPos(centerx - health_textx/2, cury, 0, health_text)
		cury = cury + fontHeight + spacer*2
	end

	if unit.attack then
		SetQuadVisible(true, attack_icon)
		SetPos(curx , cury, -2, attack_icon)
		UpdateQuadZ(attack_icon)

		local attack_textx, fontHeight = SetText(unit.attack, attack_text)
		SetPos(centerx - attack_textx/2, cury, 0, attack_text)
		cury = cury + fontHeight + spacer
	end
	
	if unit.defense then
		SetQuadVisible(true, defense_icon)
		SetPos(curx , cury, -2.78, defense_icon)
		UpdateQuadZ(defense_icon)

		local defense_textx, fontHeight = SetText(unit.defense, defense_text)
		SetPos(centerx - defense_textx/2, cury, 0, defense_text)
		cury = cury + fontHeight + spacer
	end
end

function UpdateUnitDisplayInfo()
	if current_unit then SetUnitInfoDisplay(current_unit) end
end

function DisplayUnitInfo(unit)
	if not (current_unit == unit) then ZeroUnitInfoDisplay() end
	if unit then SetUnitInfoDisplay(unit) end
end
