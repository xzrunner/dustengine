local base = require "shaderlab.nodes.floats"

local M = setmetatable({}, base)
M.__index = M

local ID = 1

function M:initialize(x, y)
	base.initialize(self, x, y, 4)
	self.name = "float4_" .. ID
	ID = ID + 1	
	self.datatype = "vec4"	
end

return M