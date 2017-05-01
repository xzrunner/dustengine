require "dust.gui"
local base = require "shaderlab.nodes.base"
local interface = require "shaderlab.nodes.interface"

local M = setmetatable({}, base)
M.__index = M

local WIDTH = 100
local TOT_HEIGHT = 75
local ITEM_HEIGHT = 25

function M:initialize(x, y)
	base.initialize(self)

	self.type = "substract"

	local frame = gui.create("frame")
	self.output = interface.create(frame, self, "out", "")	
	frame:SetTitle("Substract"):SetSize(WIDTH, TOT_HEIGHT):SetPos(x, y)
	self.frame = frame

	local panel = gui.create("panel", frame)
	self.input[1] = interface.create(panel, self, "in")	
	panel:SetSize():SetSize(WIDTH, ITEM_HEIGHT):SetPos(0, ITEM_HEIGHT)
	local text = gui.create("text", panel)
	text:SetPos(12, 5):SetText("Operand A")

	local panel = gui.create("panel", frame)
	self.input[2] = interface.create(panel, self, "in")	
	panel:SetSize():SetSize(WIDTH, ITEM_HEIGHT):SetPos(0, ITEM_HEIGHT+ITEM_HEIGHT)
	local text = gui.create("text", panel)
	text:SetPos(12, 5):SetText("Operand B")		
end

return M