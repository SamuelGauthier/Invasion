info = {
	name = "Interface",
	entry_point = "Start",
}

local width, height
local spacer = 20
Interface = {}
Button = {}
Panel = {}
ProgressBar = {}
local buttons = {}
local clicked_button = nil 
local focus_button = nil
local Control = {posx = 0, posy = 0, sizex = 0, sizey = 0, theme = {}, id = 0}
local tooltiptext, tooltip_q, tooltip, tooltip_w, tooltip_h
---------------------------------------------------------
-- Interface
---------------------------------------------------------
function Interface:create(model,...)
	local o = create(model)
	if o.init then o:init(...) end
	return o
end

function Interface:destroy(o)
	if o.quad_released then -- très moche, enlevez au plus vite
		buttons[o.quad_released] = nil
	end
	if o.destroy then o:destroy() end
end

function Interface:setTheme(model, theme)
	model.theme = theme
end

local function focus(id)
	local button = buttons[id]
	if button then
		button:over()
		focus_button = button
	end
end

local function unfocus(id)
	local button = buttons[id]
	if button then
		if not clicked_button then button:released() end
		focus_button = nil
	end
end

local function clickdown()
	if focus_button then
		focus_button:pressed()
		clicked_button = focus_button
	end
end

local function clickup()
	if clicked_button then
		if focus_button then clicked_button:over()
		else 
			clicked_button:released()
		end

		if clicked_button.signal then SendSignal(clicked_button.signal, 0, clicked_button) end

		clicked_button = nil
	end
end
---------------------------------------------------------
-- Tooltip
---------------------------------------------------------
function initToolTip(img, font)
	tooltip_q  = CreateQuad(img)
	tooltiptext = CreateTextBuffer(font)
end

function createToolTip(x, y, text)
	removeToolTip()
	
	tooltip, tooltip_w, tooltip_h = CreateQuadInstance(tooltip_q)
	Move(x - tooltip_w, y - tooltip_h, -1.5, 0, tooltip)
	
	local tx, ty = SetText(text, tooltiptext)
	SetPos(x - tooltip_w + (tooltip_w - tx)/2, y - tooltip_h + (tooltip_h - ty)/2, 0, tooltiptext)

	UpdateQuadZ(tooltip)
end

function removeToolTip()
	if tooltip then
		DeleteQuadInstance(tooltip)
		SetText("", tooltiptext)
		tooltip = nil
	end
end
---------------------------------------------------------
-- ProgressBar
---------------------------------------------------------
function ProgressBar:init()
	self.frame, self.sizex, self.sizey = CreateQuadInstance(ProgressBar.theme.frame)
	self.bar,_,self.bary = CreateQuadInstance(ProgressBar.theme.bar)
	self.value = 0
	self.id = self.bar

	Resize(0, self.bary, self.bar)
end

function ProgressBar:setValue(v)
	if v > 1 then self.value = 1
	elseif v < 0 then self.value = 0
	else self.value = v end

	Resize(self.sizex * self.value , self.bary, self.bar)
end

function ProgressBar:move(x, y)
	self.posx = x
	self.posy = y

	SetPos(x, y, -4, self.bar)
	SetPos(x, y, -3.5, self.frame)

	UpdateQuadZ(self.bar)
	UpdateQuadZ(self.frame)
end

function ProgressBar:destroy()
	DeleteQuadInstance(self.bar)
	DeleteQuadInstance(self.frame)
end

function ProgressBar:hide()
	SetQuadVisible(false, self.bar)
	SetQuadVisible(false, self.frame)
end

function ProgressBar:show()
	SetQuadVisible(true, self.bar)
	SetQuadVisible(true, self.frame)
end
---------------------------------------------------------
-- Panel
---------------------------------------------------------
function Panel:init(img)
	local q = CreateQuad(img)
	self.back, self.sizex, self.sizey = CreateQuadInstance(q)
	self.curx = spacer
	self.cury = spacer
	self.id = self.back
	self.child = {}
	self.spacerx = 5
	self.spacery = 10
	SetPos(0, 0, -10, self.back)
end

function Panel:align(sides, op_spacer)
	if string.find(sides, "center") then 
		self.posx = (width - self.sizex)/2
		self.posy = (height - self.sizey)/2
	end
	if string.find(sides, "right") then self.posx = width - self.sizex - (op_spacer or spacer)
	elseif string.find(sides, "left") then  self.posx = (op_spacer or spacer) end

	if string.find(sides, "down") then self.posy = height - self.sizey - (op_spacer or spacer)
	elseif string.find(sides, "up") then self.posy = (op_spacer or spacer) end

	SetPos(self.posx, self.posy, -10, self.back)
	self.curx = spacer
	self.cury = spacer

	for _,ctrl in ipairs(self.child) do
		self:position(ctrl)
	end
end

function Panel:insert(ctrl)
	self:position(ctrl)
	self.child[ctrl.id] = ctrl
end

function Panel:deleteChilds()
	for _,ctrl in pairs(self.child) do
		ctrl:move(-1000, -1000)   -- quick and dirty way
	end
	self.curx = spacer
	self.cury = spacer
end

function Panel:position(ctrl)
	if self.sizex - (self.curx + spacer) < ctrl.sizex then
		self.curx = spacer
		self.cury = self.cury + ctrl.sizey + self.spacery
	end

	local newposx = self.curx + self.posx
	local newposy = self.cury + self.posy

	self.curx = self.curx + self.spacerx + ctrl.sizex

	if ctrl.move then ctrl:move(newposx, newposy) end
end

function Panel:destroy()
	DeleteQuadInstance(self.back)
end
---------------------------------------------------------
-- Button
---------------------------------------------------------
function Button:init(r, p, o)
 	if r and p and o then
		Button.theme.released = CreateQuad(r)
		Button.theme.pressed = CreateQuad(p)
		Button.theme.over = CreateQuad(o)
	end

	self.quad_released, self.sizex, self.sizey = CreateQuadInstance(Button.theme.released)
	self.quad_pressed = CreateQuadInstance(Button.theme.pressed)
	self.quad_over = CreateQuadInstance(Button.theme.over)
	self.id = self.quad_released

	self:released()

	InsertMQuad(self.quad_released)

	buttons[self.quad_released] = self
end

function Button:move(px, py)
	self.posx = px
	self.posy = py
	
	SetPos(self.posx, self.posy, -2, self.quad_pressed)
	SetPos(self.posx, self.posy, -2, self.quad_over)
	SetPos(self.posx, self.posy, -2, self.quad_released)
	self:released()

	UpdateQuadZ(self.quad_pressed)
	UpdateQuadZ(self.quad_over)
	UpdateQuadZ(self.quad_released)
end

function Button:pressed()
	SetQuadVisible(true, self.quad_pressed)
	SetQuadVisible(false, self.quad_over)
	SetQuadVisible(false, self.quad_released)
end

function Button:released()
	if self.tooltip then removeToolTip() end
	SetQuadVisible(false, self.quad_pressed)
	SetQuadVisible(false, self.quad_over)
	SetQuadVisible(true, self.quad_released)
end

function Button:over()
	if self.tooltip then createToolTip(self.posx - 20, self.posy - 20, self.tooltip)  end
	SetQuadVisible(false, self.quad_pressed)
	SetQuadVisible(true, self.quad_over)
	SetQuadVisible(false, self.quad_released)
end

function Button:setSignal(sig)
	self.signal = sig
end

function Button:putText(tb, tbx, tby)
	SetPos(self.posx + (self.sizex - tbx)/2, self.posy + (self.sizey - tby)/2, -2, tb)
end

function Button:destroy()
	DeleteMQuad(self.quad_released)
	DeleteQuadInstance(self.quad_pressed)
	DeleteQuadInstance(self.quad_over)
	DeleteQuadInstance(self.quad_released)

	if focus_button == self then focus_button = nil end
end

function Button:setToolTip(text)
	self.tooltip = text
end
---------------------------------------------------------
-- Init Function
---------------------------------------------------------
function Start()
	width, height = GetResolution()
	herit(Panel, Control)
	herit(Button, Control)
	herit(ProgressBar, Control)

	AttachGlobal(focus, 301)
	AttachGlobal(unfocus, 302)

	AttachGlobal(clickdown, 33)
	AttachGlobal(clickup, 34)
end
