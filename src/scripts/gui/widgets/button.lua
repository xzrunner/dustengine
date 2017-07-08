local base = require "gui.widgets.base"

local M = setmetatable({}, base)
M.__index = M

function M:initialize()
	base.initialize(self)

	self.type = "button"
	self.text = "Button"
	self.state = "released"
	self.width = 80
	self.height = 25
	self.OnClick = nil
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
		drawfunc = skin.DrawButton
	end
	drawfunc(self)
end

function M:MousePressed(x, y)
	local ret = false
	if not self.visible then return ret end

	if self:Touch(x, y) then
		self.state = "pressed"
		ret = true
	end
	return ret
end

function M:MouseReleased(x, y)
	local ret = false
	if not self.visible then return ret end

	if self.state ~= "pressed" then return ret end

	if self:Touch(x, y) then
		self.state = "released"
		self.OnClick(self)
		ret = true
	end	
	return ret
end

function M:MouseMoved(x, y)
	local ret = false
	if not self.visible then return ret end
	
	if self.state ~= "pressed" then return ret end

	if self:Touch(x, y) then
		self.state = "pressed"
		ret = true
	else
		self.state = "released"
	end
	return ret
end

function M:KeyDown(key)
	return false
end

function M:KeyUp(key)
	return false
end

function M:SetText(text)
	self.text = text
	return self
end

function M:GetText(text)
	return self.text
end

function M:GetState()
	return self.state
end

function M:Touch(x, y)
	return gui.math.PosInRect(x,y,self.x,self.y,self.width,self.height)
end

return M