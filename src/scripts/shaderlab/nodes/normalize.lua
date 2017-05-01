require "dust.gui"
local base = require "shaderlab.nodes.base"
local interface = require "shaderlab.nodes.interface"
local parser = require "shaderlab.grammarparser2"

local M = setmetatable({}, base)
M.__index = M

local ID = 1

function M:initialize(x, y)
	base.initialize(self)

	self.type = "normalize"
	self.name = "normalize_" .. ID
	ID = ID + 1

	local frame = gui.create("frame")
	self.input[1] = interface.create(frame, self, "in")		
	self.output = interface.create(frame, self, "out", "")
	frame:SetTitle(" Normalize"):SetSize(100, 25):SetPos(x, y)
	self.frame = frame
end

function M:ToCode()
	local in1 = self.input[1].connected[1]
	if in1 then
		local datatype = self:GetDataType()
		local name = in1:GetNode():GetName()
		return datatype .. " " .. self.name .. " = normalize(" .. name .. ");" 
	else
		return ""
	end
end

function M:GetDataType()
	local in1 = self.input[1].connected[1]
	if in1 then
		return in1:GetNode():GetDataType()
	else
		return nil
	end 
end

function M:Connected()
	local in1 = self.input[1].connected[1]
	if in1 then
		parser.add(self, in1:GetNode())
	end	
end

function M:Deconnected()
	parser.remove(self.name)
end

return M