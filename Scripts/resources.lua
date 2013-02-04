info = {
	name = "Resources",
	entry_point = "",
}

local wood_icon
local food_icon
local gold_icon
local stone_icon

font14 = LoadFont("poor-richard-2.ttf", 14)
resource = {}
local resource_text = {}

function initBarResources(panel)
	wood_icon = LoadTex("wood.png")
	food_icon = LoadTex("food.png")
	gold_icon = LoadTex("gold.png")
	stone_icon = LoadTex("stone.png")

	for i=1,4 do
		resource[i] = 500
		resource_text[i] = CreateTextBuffer(font14)
		
		local _,textposy = SetText(resource[i], resource_text[i])
		SetPos(panel.posx + panel.spacerx + i*90 + 200, panel.posy + panel.spacery , 0, resource_text[i])
	end

	SetPos(panel.posx + panel.spacerx + 90 + 170, panel.posy + panel.spacery, -2, food_icon)
	SetPos(panel.posx + panel.spacerx + 2*90 + 170, panel.posy + panel.spacery, -2, wood_icon)
	SetPos(panel.posx + panel.spacerx + 3*90 + 170, panel.posy + panel.spacery, -2, gold_icon)
	SetPos(panel.posx + panel.spacerx + 4*90 + 170, panel.posy + panel.spacery, -2, stone_icon)
	UpdateQuadZ(wood_icon)
	UpdateQuadZ(food_icon)
	UpdateQuadZ(gold_icon)
	UpdateQuadZ(stone_icon)
end

function updateBarResources()
	for i=1,4 do
		SetText(resource[i], resource_text[i])
	end
end

function buy(udef)
	if udef.wood_cost <= resource[2] and 
	   udef.food_cost <= resource[1] and 
	   udef.gold_cost <= resource[3] and 
	   udef.stone_cost <= resource[4] then
		resource[1] = resource[1] - udef.food_cost
		resource[2] = resource[2] - udef.wood_cost
		resource[3] = resource[3] - udef.gold_cost
		resource[4] = resource[4] - udef.stone_cost
		updateBarResources()
		return true
	end	
	return false
end
