require "gui"
local base = require "shaderlab.nodes.base"
local interface = require "shaderlab.nodes.interface"

local M = setmetatable({}, base)
M.__index = M

function M:initialize(x, y)
	base.initialize(self)

	self.type = "cameratoview"

	self.name = "Projection"
	self.datatype = "mat4"

	local frame = gui.create("frame")
	self.output = interface.create(frame, self, "out", "")
	frame:SetTitle("Camera To View Matrix"):SetSize(200, 25):SetPos(x, y)
	self.frame = frame
end

function M:ToCode()
	return self.name
end

return M