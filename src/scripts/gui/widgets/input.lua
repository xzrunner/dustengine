local base = require "gui.widgets.base"

local M = setmetatable({}, base)
M.__index = M

local BACKSPACE = 8
local DOT       = 190
local KEY_0		= 48
local KEY_9		= 57
local KEY_A     = 65
local KEY_Z     = 90

local TYPE_NUMBER = 0
local TYPE_STRING = 1

function M:initialize()
	base.initialize(self)

	self.type = "input"
	self.width = 100
	self.height = 18
	self.text = ""
	self.font = 14
	self.format = TYPE_STRING
end

function M:Update()
	if not self.visible then return end
	
	local update = self.UpdateCB
	if update then
		update(self)
	end	
end

function M:Draw()
	if not self.visible then return end

	local drawfunc = self.DrawCB
	if not drawfunc then
		local skin = self:GetSkin()
		drawfunc = skin.DrawInput
	end
	drawfunc(self)
end

function M:MousePressed(x, y)
	local ret = false
	if not self.visible then return ret end

	if self:Touch(x, y) then
		gui.input_focus = self
		ret = true
	end

	return ret
end

function M:MouseReleased(x, y)
	local ret = false
	if not self.visible then return ret end
	return ret
end

function M:MouseMoved(x, y)
	local ret = false
	if not self.visible then return ret end
	return ret
end

function M:KeyDown(key)
	-- print(key)
	if key == BACKSPACE then
		if self.text and #self.text > 0 then
			self.text = string.sub(self.text, 1, #self.text-1)
		end
	elseif key == DOT and self.format == TYPE_NUMBER then
		self.text = self.text .. "."
	elseif (self.format == TYPE_NUMBER and key >= KEY_0 and key <= KEY_9) or 
		   (self.format == TYPE_STRING and key >= KEY_A and key <= KEY_Z) then
		self.text = self.text .. string.char(key)
	end
	return false
end

function M:KeyUp(key)
	return false
end

function M:SetText(text)
	if self.format == TYPE_NUMBER then
		if tonumber(text) then
			self.text = text
		end
	elseif self.format == TYPE_STRING then
		if tostring(text) then
			self.text = text
		end
	end
	return self
end

function M:GetText()
	return self.text
end

function M:SetFont(font)
	self.font = font
	return self
end

function M:GetFont()
	return self.font
end

function M:SetType(format)
	if format == "string" then
		self.format = TYPE_STRING
	elseif format == "number" then
		self.format = TYPE_NUMBER
	end
	return self
end

function M:Touch(x, y)
	return gui.math.PosInRect(x,y,self.x,self.y,self.width,self.height)
end

return M