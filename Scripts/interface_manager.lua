info = {
	name = "InterfaceManager",
	entry_point = "Start",
}

local function init()
	initToolTip("tooltip.bmp", font12)
	rdpanel = Interface:create(Panel, "menuright.tif")
	rdpanel:align("rightdown")

	ldpanel = Interface:create(Panel, "menuleft.tif")
	ldpanel:align("leftdown")
	InitUnitInfoDisplay(ldpanel)

	toppanel = Interface:create(Panel, "menutop.tif")
	toppanel:align("centerup", 0)

	minimap = Interface:create(Panel, "Minimap.tif")
	minimap:align("leftup")
	
	counterpanel = Interface:create(Panel, "counter.tif")
	counterpanel:align("rightup")
	
	initBarResources(toppanel)

	createButtons()
end

local function unit_icon_click(button)
	if nb_selected > 0 then
		local unit = units[selected_units[1]]
		local newUnitDef = getUnitDef(button.name)
		if newUnitDef then
			if not unit.canMove and newUnitDef.canMove then
				SendSignal(104, unit.id, newUnitDef)
			elseif unit.canMove and not newUnitDef.canMove then
				SendSignal(73, 0, unit, newUnitDef)
			end
		else
			print("tried to spawn a " .. button.name .. " but failed")
		end
	end
end

function Start()
	AttachGlobal(init, 10)
	AttachGlobal(unit_icon_click, 40)
end
