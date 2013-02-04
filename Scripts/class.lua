info = {
	name = "Class",
	group = "Class",
	depedency = "",
	entry_point = "",
}

function herit(o, model) setmetatable(o, { __index = model}) end
function create(model)
	local o = {}
	setmetatable(o, {__index = model})
	return o
end
