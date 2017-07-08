require "gui"
local base = require "shaderlab.nodes.base"
local interface = require "shaderlab.nodes.interface"

local M = setmetatable({}, base)
M.__index = M

local function _init_static(self)
	local attr = {}
	attr.type = self.type
	attr.name = 'Normal'	
	table.insert(self.attributes, attr)
end

function M:initialize(x, y)
	base.initialize(self)

	self.type = "vertexnormal"

	self.name = "Normal"
	self.datatype = "vec3"

	self.attributes = {}
	_init_static(self)	

	local frame = gui.create("frame")
	self.output = interface.create(frame, self, "out", "")
	frame:SetTitle("Vertex Normal"):SetSize(130, 25):SetPos(x, y)
	self.frame = frame
end

function M:ToCode()
	return self.name
end

return M