local base = require "dust.gui.widgets.base"

local M = setmetatable({}, base)
M.__index = M

function M:initialize()
	base.initialize(self)

	self.type = "slider"
	self.slidetype = "horizontal"
	self.width = 5
	self.height = 5
	self.max = 10
	self.min = 0
	self.value = 0
	self.decimals = 2
	self.buttonwidth = 10
	self.buttonheight = 20
	self.OnValueChanged	= nil
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
		drawfunc = skin.DrawSlider
	end
	drawfunc(self)
end

function M:MousePressed(x, y)
	local ret = false
	if not self.visible then return ret end

	self.pressed = false

	local _x = self.x
	local _y = self.y
	local w = self.width
	local h = self.height
	local bh = self.buttonheight
	if self.slidetype == "horizontal" then
		local center = _y + h * 0.5
		if x > _x and x < _x + w and
		   y > center - bh * 0.5 and y < center + bh * 0.5 then
		   self.pressed = true
		   local percent = (x - _x) / w
		   self:SetValue(percent)
		   ret = true
		end
	else
		local center = _x + w * 0.5
		if y > _y and y < _y + h and
		   x > center - bh * 0.5 and x < center + bh * 0.5 then
		   self.pressed = true
		   local percent = (y - _y) / h
		   self:SetValue(percent)
		   ret = true
		end
	end

	return ret
end

function M:MouseReleased(x, y)
	local ret = false
	if not self.visible then return ret end

	self.pressed = false

	return ret
end

function M:MouseMoved(x, y)
	local ret = false
	if not self.visible then return ret end

	if self.pressed then
		local _x = self.x
		local _y = self.y
		local w = self.width
		local h = self.height
		if self.slidetype == "horizontal" then
			if x > _x and x < _x + w then
				local percent = (x - _x) / w
				self:SetValue(percent)
				ret = true
			end
		else
			if y > _y and y < _y + h then
				local percent = (y - _y) / h
				self:SetValue(percent)
				ret = true
			end
		end		
	end

	return ret
end

function M:KeyDown(key)
	return false
end

function M:KeyUp(key)
	return false
end

function M:SetSlideType(slidetype)
	self.slidetype = slidetype
	return self
end

function M:GetSlideType()
	return self.slidetype
end

function M:SetMinMax(min, max)
	self.min = min
	self.max = max
	if self.value > self.max then
		self.value = self.max
	end
	if self.value < self.min then
		self.value = self.min
	end
	return self
end

function M:SetMin(min)
	self.min = min
	if self.value < self.min then
		self.value = self.min
	end
	return self	
end

function M:SetMax(max)
	self.max = max
	if self.value > self.max then
		self.value = self.max
	end
	return self
end

function M:GetMinMax()
	return self.min, self.max
end

function M:SetValue(percent)
	local value = self.min + (self.max - self.min) * percent
	self.value = gui.math.Round(value, self.decimals)

	local onvaluechanged = self.OnValueChanged
	if onvaluechanged then
		onvaluechanged(self.value)
	end
end

function M:GetValue()
	return self.value
end

function M:SetDecimals(decimals)
	self.decimals = decimals
	return self
end

function M:SetButtonSize(width, height)
	self.buttonwidth = width
	self.buttonheight = height
	return self
end

function M:GetButtonSize()
	return self.buttonwidth, self.buttonheight
end

return M