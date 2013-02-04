info = {
	name = "MessageBox",
	entry_point = "Start",
}

local msgbox_text
local msgbox_q
local back_q
local info_text

function InitMsgBox(img, font)
	msgbox_q = CreateQuad(img)
	msgbox_text = CreateTextBuffer(font)
	info_text = CreateTextBuffer(font)
	back_q = CreateQuad("black.bmp")
end

function MsgBox(text) -- must be called inside a thread
	myprint("try to create")
	local width, height = GetResolution()
	local msgbox, msgbox_w, msgbox_h = CreateQuadInstance(msgbox_q)
	local msgbox_x = (width-msgbox_w)/2
	local msgbox_y = (height-msgbox_h)/2
	SetPos(msgbox_x, msgbox_y, -1.1, msgbox)
	
	local tx, ty = SetText(text, msgbox_text)
	SetPos(msgbox_x + (msgbox_w - tx)/2, msgbox_y + (msgbox_h - ty)/2, 0, msgbox_text)
	UpdateQuadZ(msgbox)
	
	local back = CreateQuadInstance(back_q)
	Resize(width, height, back)
	ChangeOpacity(0.7, 0, back)
	SetPos(0, 0, -1.2, back)
	
	
	tx, ty = SetText("Press any key to continue", info_text)
	SetPos((width-tx)/2, height - ty - 20, 0, info_text)
	
	UpdateQuadZ(back)
	
	TogglePause()
	Sleep(0)
	WaitForKey()
	TogglePause()
	DeleteQuadInstance(msgbox)
	DeleteQuadInstance(back)
	SetText("", info_text)
	SetText("", msgbox_text)
end

function Start()
end