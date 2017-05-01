local Graphics = require "dust.graphics"
local util = require "shaderlab.util"

local M = {}
M.__index = M

local RADIUS        = 8
local NODE_COLOR    = { 83, 255, 255, 255}
local PANEL_COLOR   = { 44,  44,  44, 255}
local BORDER_COLOR  = { 32,  32,  32, 255}
local BORDER_WIDTH  = 1.5

function M.create(parent, node, type, datatype)
	local obj = setmetatable({}, M)
	obj:initialize(parent, node, type, datatype)
	return obj
end

function M:initialize(parent, node, type, datatype)
	assert(type == "in" or type == "out", "interface:initialize() type error!")
	self.parent = parent
	self.node = node
	self.type = type
	self.datatype = datatype
	self.connected = {}
end

function M:Draw()
	local x, y = self.parent:GetPos()
	local w, h = self.parent:GetSize()
	if self.type == "out" then
		Graphics.set_color(PANEL_COLOR)
		Graphics.draw_rect(true, x+w, y+RADIUS*0.5, RADIUS, h-RADIUS)		
	end

	local nx, ny = self:GetPos()
	Graphics.set_color(NODE_COLOR)
	Graphics.circle(true, nx, ny, RADIUS, 8)
	Graphics.set_color(BORDER_COLOR)
	Graphics.set_linewidth(BORDER_WIDTH)
	Graphics.circle(false, nx, ny, RADIUS, 16)
end

function M:Touch(x, y)
	local nx, ny = self:GetPos()
	return gui.math.PosInCircle(x, y, nx, ny, RADIUS)
end

function M:GetPos()
	local x, y = self.parent:GetPos()
	local w, h = self.parent:GetSize()
	local nx, ny
	if self.type  == "in" then
		nx = x
		ny = y+h*0.5
	else
		nx = x+w+RADIUS
		ny = y+h*0.5		
	end
	return nx, ny
end

function M:GetNode()
	return self.node
end

function M:Connect(interface)
	table.insert(self.connected, interface)
	table.insert(interface.connected, self)
end

function M:Deconnect(interface)
	if interface then
		util.table_remove_value(self.connected, interface)
		util.table_remove_value(interface.connected, self)
	else
		for _, v in pairs(self.connected) do
			util.table_remove_value(v.conndeted, self)
		end
		self.connected = {}		
	end
end

return M