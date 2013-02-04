info = {
	name = "Signal",
	group = "Signal",
	depedency = "",
	entry_point = "",
}

globalFuns = {}

--------------------------------------------------
-- Signal related
--------------------------------------------------
function SendSignal(sig, unitID,...)
	local oldThread = activeThread
	activeThread = nil
	activeUnitID = nil
	if unitID and unitID > 0 then
		local unit = units[unitID]
		if unit.funs[sig] then
			activeUnitID = unit.id
			KillTasks(unitID)
			unit.funs[sig](unit,...)
		end
	else
		local fun_table = globalFuns[sig]
		if fun_table then
			for _,fun in pairs(fun_table) do
				fun(...)
			end
		end
	end
	activeThread = oldThread
end

function AttachGlobal(fun, sig)
	local table = globalFuns[sig] or {}
	table[#table+1] = fun
	globalFuns[sig] = table
end
