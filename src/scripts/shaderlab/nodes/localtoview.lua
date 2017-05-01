require "dust.gui"
local base = require "shaderlab.nodes.base"
local interface = require "shaderlab.nodes.interface"

local M = setmetatable({}, base)
M.__index = M

local function _init_static(self)
	local uniform = {}
	uniform.type = self.datatype
	uniform.name = self.name
	table.insert(self.uniforms, uniform)
end

function M:initialize(x, y)
	base.initialize(self)

	self.type = "localtoview"

	self.datatype = "mat4"
	self.name = "LocalToView"

	self.uniforms = {}
	_init_static(self)		

	local frame = gui.create("frame")
	self.output = interface.create(frame, self, "out", "")
	frame:SetTitle("Local To View Matrix"):SetSize(200, 25):SetPos(x, y)
	self.frame = frame
end

function M:ToCode()
	-- return self.name
	return "LocalToView = Projection * Modelview;"
end

return M