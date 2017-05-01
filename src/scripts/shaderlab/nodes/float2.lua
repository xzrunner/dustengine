local base = require "shaderlab.nodes.floats"

local M = setmetatable({}, base)
M.__index = M

local ID = 1

function M:initialize(x, y)
	base.initialize(self, x, y, 2)
	self.name = "float2_" .. ID
	ID = ID + 1		
	self.datatype = "vec2"
end

return M