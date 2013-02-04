info = {
	name = "searchDeposit",
	group = "SearchDeposit",
	depedency = "UnitCreation",
	entry_point = "",
}

function searchDeposit(ref)
	local nearest = nil
	local nearestDistance = 0
	local refx, refz = GetPos(ref)
	for _,unit in pairs(units) do
		if unit.deposit then
			local x,z = GetPos(unit.id)
			local distance = (refx-x)*(refx-x) + (refz-z)*(refz-z)
			if not nearest or nearestDistance > distance then
				nearest = unit
				nearestDistance = distance
			end
		end
	end
	return nearest
end
