require "gui"
local base = require "shaderlab.nodes.base"
local interface = require "shaderlab.nodes.interface"

local M = setmetatable({}, base)
M.__index = M

local TOT_HEIGHT  = 75
local ITEM_WIDTH  = 55
local ITEM_HEIGHT = 20

local ID = 1

function M:initialize(x, y, num)
	base.initialize(self)

	self.type = "float" .. num
	self.name = "floats_" .. ID
	ID = ID + 1

	local frame = gui.create("frame")
	self.output = interface.create(frame, self, "out", "")
	frame:SetTitle("Float " .. num):SetSize(60+60*num, TOT_HEIGHT):SetPos(x, y)
	self.frame = frame

	local text = gui.create("text", frame)
	text:SetText("Value"):SetPos(5, 27)

	self.value = {}
	for i = 1, num do
		local v = gui.create("input", frame)
		v:SetPos(60 + (ITEM_WIDTH+5)*(i-1), 27):SetSize(ITEM_WIDTH, ITEM_HEIGHT)
		v:SetType("number"):SetText("1")
		self.value[i] = v
	end

	local name_title = gui.create("text", frame)
	name_title:SetText("Name"):SetPos(5, 55)
	local name_ctrl = gui.create("input", frame)
	name_ctrl:SetPos(60, 50):SetSize(ITEM_WIDTH*num+5*(num-1), ITEM_HEIGHT)
	name_ctrl:SetType("string"):SetText("")
	self.name_ctrl = name_ctrl
end

function M:ToCode()
	return self.name
end

function M:SetName(name)
	base.SetName(self, name)
	self.name_ctrl:SetText(name)
	for i = 1, #self.uniforms do
		local uniform = self.uniforms[i]
		uniform.name = name
	end
	return self
end

return M