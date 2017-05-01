local base = require "dust.gui.widgets.base"
local Graphics = require "dust.graphics"

local M = setmetatable({}, base)
M.__index = M

local COLOR_TEXT = {"Red", "Green", "Blue"}

function M:initialize()
	base.initialize(self)

	self.type = "colorchanger"
	self.color = {0, 0, 0, 255}
	self.width = 20
	self.height = 20	
	self.children = {}
	self.OnColorChanged	= nil

	self:InitLayout()
end

function M:Update()
	if not self.visible then return end

	for k, v in ipairs(self.children) do
		v:Update()
	end
end

function M:Draw()
	if not self.visible then return end

	for k, v in ipairs(self.children) do
		v:Draw()
	end
end

function M:MousePressed(x, y)
	local ret = false
	if not self.visible then return ret end

	for k, v in ipairs(self.children) do
		local skip = v:MousePressed(x, y)
		if skip then return true end
	end	
	return ret
end

function M:MouseReleased(x, y)
	local ret = false
	if not self.visible then return ret end

	for k, v in ipairs(self.children) do
		local skip = v:MouseReleased(x, y)
		if skip then return true end
	end	
	return ret
end

function M:MouseMoved(x, y)
	local ret = false
	if not self.visible then return ret end

	for k, v in ipairs(self.children) do
		local skip = v:MouseMoved(x, y)
		if skip then return true end
	end	
	return ret
end

function M:KeyDown(key)
	return false
end

function M:KeyUp(key)
	return false
end

function M:Refresh()
	base.Refresh(self)

	for k, v in ipairs(self.children) do
		v:Refresh()
	end	
end

function M:SetColor(color)
	self.color[1] = color[1]
	self.color[2] = color[2]
	self.color[3] = color[3]
	self.color[4] = color[4] or 255

	self:SetColorSlider(1, color[1])
	self:SetColorSlider(2, color[2])
	self:SetColorSlider(3, color[3])
end

function M:InitLayout()
	self.sliders = {}
	
	self:CreateColorPanel()
	self:CreateColorSlider(1)
	self:CreateColorSlider(2)
	self:CreateColorSlider(3)	
end

function M:CreateColorPanel()
	local colorbox = gui.create("panel", self)
	colorbox:SetPos(5, 30)
	colorbox:SetSize(490, 100)
	colorbox.DrawCB = function(object)
		local x = object:GetX()
		local y = object:GetY()
		local w = object:GetWidth()
		local h = object:GetHeight()
		Graphics.set_color(self.color)
		Graphics.draw_rect(true, x, y, w, h)
		Graphics.set_color(143, 143, 143, 255)
		Graphics.draw_rect(false, x, y, w, h)
	end	
end

function M:CreateColorSlider(index)
	local offset = 40 * (index - 1)

	local slidername = gui.create("text", self)
	slidername:SetPos(5, 135 + offset)
	local text = COLOR_TEXT[index]
	slidername:SetText(text)
	
	local slidervalue = gui.create("text", self)
	slidervalue:SetPos(470, 135 + offset)

	local slider = gui.create("slider", self)
	slider:SetPos(5, 150 + offset)
	slider:SetWidth(490)
	slider:SetMax(255)
	slider:SetDecimals(0)
	slider.OnValueChanged = function(value)
		local oncolorchanged = self.OnColorChanged
		if oncolorchanged then
			oncolorchanged(self.color)
		end
		self.color[index] = value
	end
	self.sliders[index] = slider

	slidervalue.UpdateCB = function(object)
		object:SetText(slider:GetValue())
	end		
end

function M:SetColorSlider(index, color)
	local slider = self.sliders[index]
	slider:SetValue(color / 255)
end

return M