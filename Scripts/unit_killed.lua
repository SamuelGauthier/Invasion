info = {
	name = "UnitKilled",
	entry_point = "Init",
}

local function killed(unit)
	if not unit.death_process then
		unit.death_process = true
		if nb_selected > 0 and selected_units[1] == unit.id then
			nb_selected = nb_selected - 1
			selected_units[1] = nil
			ZeroUnitInfoDisplay()
		end
		units[unit.id] = nil
		KillTasks(unit.id, true)
		UnregisterUnit(unit.id)
	end
end

function Init()
	AttachGlobal(killed, 103)
end
