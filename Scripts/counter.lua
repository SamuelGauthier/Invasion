info = {
	name = "Counter",
	entry_point = "Start",
}

local font30 = LoadFont("poor-richard-2.ttf", 30)

local countertext
local countervalue

local function countertick()
	while countervalue >= 0 do
		local tx,ty = SetText(countervalue, countertext)
		SetPos(counterpanel.posx + (counterpanel.sizex-tx)/2, counterpanel.posy  + (counterpanel.sizey-ty)/2, 0, countertext)
		Sleep(1)
		countervalue = countervalue - 1
	end
	
	SendSignal(99) -- send barbarians
end

local function startcounter()
	countertext = countertext or CreateTextBuffer(font30)
	countervalue = 200
	StartThread(countertick, false)
end

function Start()
	AttachGlobal(startcounter, 67)
end