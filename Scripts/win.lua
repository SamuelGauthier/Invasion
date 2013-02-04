info = {
	name = "Win",
	entry_point = "Init",
}

local function youre_won()
	MsgBox("You won!")
end

local function won()
	StartThread(youre_won, false)
end

function Init()
	AttachGlobal(won, 77)
end
