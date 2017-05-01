local base = require "shaderlab.nodes.floats"

local M = setmetatable({}, base)
M.__index = M

function M:initialize(x, y)
	base.initialize(self, x, y, 1)
	self.datatype = "float"
end

function M:GetName()
	return self.name or tostring(self.value)
end

function M:SetValue(v)
	self.value = v
	return self
end

return M