info = {
	name = "Quads",
	entry_point = "",
}

function LoadTex(name)
	return CreateQuadInstance(CreateQuad(name))
end
