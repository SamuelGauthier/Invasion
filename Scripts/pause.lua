info = {
	name = "Pause",
	entry_point = "Init",
}

local function pause_process(key)
	if key == 112 then		-- 112 : p
		MsgBox("Pause")
	end
end

------------------------------------------------------------------
local function pause(key)
	myprint("pause " .. key)
	StartThread(pause_process, false, key)
end

function Init()
	AttachGlobal(pause, 37)
end
