require "gui"
local base = require "shaderlab.nodes.base"
local interface = require "shaderlab.nodes.interface"
local shader = require "shaderlab.shader"
-- local c = require "ejoy3d.render.c"
local util = require "shaderlab.util"

local M = setmetatable({}, base)
M.__index = M

local WIDTH = 160
local TOT_HEIGHT = 75
local ITEM_HEIGHT = 25

local ID = 1

function M:initialize(x, y)
	base.initialize(self)

	self.type = "rgbacolor"

	self.name = "rgba_" .. ID
	ID = ID + 1
	self.datatype = "vec4"

	self.rgb = {}
	self.rgb[1] = 255
	self.rgb[2] = 255
	self.rgb[3] = 255	
	self.alpha = 255

	local frame = gui.create("frame")
	self.output = interface.create(frame, self, "out")	
	frame:SetTitle("RGBA Color"):SetSize(WIDTH, TOT_HEIGHT):SetPos(x, y)
	self.frame = frame

	local panel = gui.create("panel", frame)
	self.input[1] = interface.create(panel, self, "in")	
	panel:SetSize():SetSize(WIDTH, ITEM_HEIGHT):SetPos(0, ITEM_HEIGHT)
	local text = gui.create("text", panel)
	text:SetPos(12, 5):SetText("Color")
	local colorbutton = gui.create("colorbutton", panel)
	colorbutton:SetPos(100, 5):SetSize(50, 15):SetColor(self.rgb)
	colorbutton.OnColorChanged = function(color)
		self.rgb = color
		self:OnColorChanged()
	end

	local panel = gui.create("panel", frame)
	self.input[2] = interface.create(panel, self, "in")	
	panel:SetSize():SetSize(WIDTH, ITEM_HEIGHT):SetPos(0, ITEM_HEIGHT+ITEM_HEIGHT)
	local text = gui.create("text", panel)
	text:SetPos(12, 5):SetText("Alpha")
	local slider = gui.create("slider", panel)
	slider:SetPos(50, 9):SetWidth(100):SetMax(255):SetDecimals(0):SetValue(self.alpha/255)
	slider.OnValueChanged = function(value)
		self.alpha = value
		self:OnColorChanged()
	end
end

-- function M:Connect(interface)
-- 	base.Connect(self, interface)

-- 	local node = interface:GetNode()
-- 	local type = node:GetType()
-- 	if type == "outputcolor" then
-- 		if self.shader then
-- 			shader.remove(false, self.shader.id)
-- 		end

-- 		shader.insert_uniform(false, "vec4", "u_color")
-- 		local sentence = node:ToCode() .. " = " .. self:ToCode() .. ";"
-- 		self.shader = shader.insert(false, sentence)
-- 		shader.insert_node(self)
-- 		shader.build()
-- 	end
-- end

-- function M:Deconnect(interface)
-- 	if not self.shader then return end

-- 	for k, v in ipairs(self.output.connected) do
-- 		if v == interface then
-- 			local node = v:GetNode()
-- 			if node:GetType() == "outputcolor" then
-- 				table.remove(self.output.connected, k)
-- 				util.table_remove_value(interface.connected, self.output)
-- 				shader.remove(false, self.shader.id)

-- 				-- local sentence = node:ToCode() .. " = vec4(1,1,1,1);"
-- 				local sentence = node:ToCode()
-- 				print(sentence)
-- 				self.shader = shader.insert(false, sentence)
-- 				shader.remove_node(self)
-- 				shader.build()
-- 				break
-- 			end
-- 		end
-- 	end
-- end

-- function M:GetName()
-- 	return "vec4(1,1,1,1);"
-- 	-- return string.format('%q', "vec4(%f,%f,%f,%f), rgb[1]/255.0,rgb[2]/255.0,rgb[3]/255.0,rgb[4]/255.0")
-- end

function M:ShaderInit()
	-- c.add_uniform(self.name)
	self:OnColorChanged()
end

function M:OnColorChanged()
	local rgb = self.rgb
	local alpha = self.alpha
	local color = {}
	color[1] = rgb[1] / 255.0
	color[2] = rgb[2] / 255.0
	color[3] = rgb[3] / 255.0
	color[4] = alpha / 255.0
	c.render_unknown_program()	
	c.set_uniform4f(self.name, color)
end

return M