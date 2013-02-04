info = {
	name = "UIUnit",
	group = "UIUnit",
	depedency = "UnitCreation",
	entry_point = "",
}

local unitButtons = {}

function icones_build(panel, unit)
	if unit.builds then
		for _,build in pairs(unit.builds) do
			if unitButtons[build] then panel:insert(unitButtons[build]) end
		end
	end
end

function createButtons()
	local ppath = "../IconsandCursors/icons"
	for _,udef in pairs(unitDefs) do
		if udef.builds then
			for _,build in pairs(udef.builds) do
				unitButtons[build] = unitButtons[build] or Interface:create(Button, ppath .. "/icone_" .. build .. ".png", ppath .. "-pressed/icone_" .. build .. ".png", ppath .. "-selected/icone_" .. build .. ".png")
				unitButtons[build].name = build
				unitButtons[build]:setSignal(40)
				unitButtons[build]:move(-1000, -1000)
				unitButtons[build]:setToolTip(build)
			end
		end
	end
end
