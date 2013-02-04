info = {
	name = "StartTransition",
	group = "StartTransition",
	depedency = "",
	entry_point = "Start",
}

local q
local qi

local function transition_loop_process()
	ChangeOpacity(0, 1, qi)
	Sleep(1)
	DeleteQuadInstance(qi)
end

local function transition_loop()
	UpdateQuadZ(qi)
	StartThread(transition_loop_process, false)
end

local function transition_begin()
	local width, height = GetResolution()
	q = CreateQuad("black.bmp")
	qi = CreateQuadInstance(q)
	Resize(width, height, qi)
	SetPos(0, 0, 0, qi)
	UpdateQuadZ(qi)
end

function Start()
	AttachGlobal(transition_begin, 6)
	AttachGlobal(transition_loop, 7)
end
