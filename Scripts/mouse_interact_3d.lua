info = {
	name = "MouseInteract",
	entry_point = "Init",
}

local last_selected

local function clickleft()
	if nb_selected > 0 then
		local unit = units[selected_units[1]]
		if not (last_selected == unit) then
			rdpanel:deleteChilds()
			icones_build(rdpanel, unit)
			DisplayUnitInfo(unit)
			last_selected = unit
		end
	end
end

local function destroy_selection_quads()
	for i=1,#selection_circles do
		DestroyQuadInstance(selection_circles[i])
		selection_circles[i] = nil
	end
end

local function create_selection_quads()
	for i=1,nb_selected do
		selection_circles[i] = CreateQuadInstance(selection_circle_quad)
		local x,_,z = GetPos(selected_units[i])
		local unit = units[selected_units[i]]
		SetPos(x - unit.boundingx, 0.1, z - unit.boundingz, selection_circles[i])
	end
end

local function clickright()
	if nb_selected > 0 then
		for i=1,nb_selected do
			local unit = units[selected_units[i]]
			if unit.canMove  and not unit.ennemy then
				if target_unit then
					local target = units[target_unit]
					if target.resource then
						SendSignal(72, unit.id, target_unit)
					elseif not target.canMove and
						target.health < target.max_health then
							SendSignal(71, unit.id, target)
					elseif target.ennemy then
						SendSignal(80, unit.id, target)
					end
				else
					local x,z = GetMouseRayPos()
					SendSignal(70, unit.id, x, z)
				end
			end
		end
	end
end

local function mouse_interact_init()
	AttachGlobal(clickleft, 34)
	AttachGlobal(clickright, 36)

	selection_circle_quad = CreateQuad("circle.png", false)
end

function Init()
	AttachGlobal(mouse_interact_init, 16)
end
