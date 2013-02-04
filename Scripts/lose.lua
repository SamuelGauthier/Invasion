info = {
	name = "Lose",
	entry_point = "Init",
}

local function youre_lost()
	MsgBox("You have been defeated")
end

local function losing_control(target)
	if target.name == "centre_ville" then
		StartThread(youre_lost, false)
	end
end

function Init()
	AttachGlobal(losing_control, 103)
end
