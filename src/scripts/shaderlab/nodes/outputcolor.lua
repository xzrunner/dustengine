require "gui"
local base = require "shaderlab.nodes.base"
local interface = require "shaderlab.nodes.interface"
local parser = require "shaderlab.grammarparser2"

local M = setmetatable({}, base)
M.__index = M

function M:initialize(x, y)
	base.initialize(self)

	self.type = "outputcolor"
	self.name = "gl_FragColor"

	local frame = gui.create("frame")
	self.input[1] = interface.create(frame, self, "in")
	frame:SetTitle(" Output Color"):SetSize(130, 25):SetPos(x, y)
	self.frame = frame
end

function M:ToCode()
	local in1 = self.input[1].connected[1]
	if in1 then
		return self.name .. " = " .. in1:GetNode():GetName() .. ";"
	else
		return ""
	end
end

function M:Connected()
	local in1 = self.input[1].connected[1]
	if in1 then
		parser.add(self, in1:GetNode())
	end	
end

function M:Deconnected()
	parser.remove(self)
end

return M