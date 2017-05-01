require "dust.gui"
local base = require "shaderlab.nodes.base"
local interface = require "shaderlab.nodes.interface"

local M = setmetatable({}, base)
M.__index = M

function M:initialize(x, y)
	base.initialize(self)

	self.type = "localtocamera"

	self.name = "Modelview"
	self.datatype = "mat4"

	local frame = gui.create("frame")
	self.output = interface.create(frame, self, "out", "")
	frame:SetTitle("Local To Camera Matrix"):SetSize(200, 25):SetPos(x, y)
	self.frame = frame
end

function M:ToCode()
	return self.name
end

return M