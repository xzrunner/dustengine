local base = require "shaderlab.nodes.floats"

local M = setmetatable({}, base)
M.__index = M

local ID = 1

local function _init_static(self)
	local uniform = {}
	uniform.type = self.datatype
	uniform.name = self.name
	table.insert(self.uniforms, uniform)
end

function M:initialize(x, y)
	base.initialize(self, x, y, 3)
	
	self.name = "float3_" .. ID
	ID = ID + 1
	self.datatype = "vec3"

	self.uniforms = {}
	_init_static(self)	
end

return M