info = {
	name = "MapLoader",
	entry_point = "Init",
}

local spread = 2
local function regroupement(grass_def, x, y)
	for i=1,5 do
		local g = grass_def:createUnit()
		Move(x + math.random()*spread-spread/2, y + math.random()*spread-spread/2, 0, g.id)
	end
end

local function loadMap(mapname)
	local image, w, h = LoadMap("map.bmp")
	local terrainSize = 16*8
	local incx = terrainSize/(w-1)
	local incy = terrainSize/(h-1)

	for x=0,w-1 do
		for y=0,h-1 do
			local str = nil
			local pix = GetPixel(image, x, y)	

			if pix == 20 then str = "arbre"
			elseif pix == 40 then str = "arbre2"
			elseif pix == 60 then str = "centre_ville"
			elseif pix == 80 then str = "herbe"
			elseif pix == 100 then str = "mine_or"
			elseif pix == 120 then str = "mine_pierre"
			elseif pix == 140 then str = "buisson"
			end

			if str then
				local udef = getUnitDef(str)
				local posx = x * incx + (-terrainSize/2)
				local posy = y * incy + (-terrainSize/2)
				if str == "herbe" then
					regroupement(udef, posx, posy)
				else
					local u = udef:createUnit()
					Move(posx, 0.001, posy, 0, u.id)
					OccupArea(u.id)
				end
			end
		end
	end
	
	UnloadMap(image)
end



local function load_terrain()
	math.randomseed(10)

	CreateTerrain(16, 8)
	LoadTerrainTextures("texmap.bmp", "Config/terrain.txt")
	ResetElapsedTime()

	loadMap("map.bmp")

	SendSignal(55) -- prepare_units
end

function Init()
	AttachGlobal(load_terrain, 9)
end
