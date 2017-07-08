local base = require "gui.widgets.base"

local M = setmetatable({}, base)
M.__index = M

function M:initialize()
	base.initialize(self)

	self.type = "checkbox"
	self.text = "Button"
	self.width = 20
	self.height = 20
	self.checked = false
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
		drawfunc = skin.DrawCheckBox
	end
	drawfunc(self)	
end

function M:MousePressed(x, y)
	local ret = false
	if not self.visible then return ret end

	if self:Touch(x, y) then
		self.pressed = true
		ret = true
	else
		self.pressed = false
	end
	return ret
end

function M:MouseReleased(x, y)
	local ret = false
	if not self.visible then return ret end
	
	if self:Touch(x, y) and self.pressed then
		self.checked = not self.checked
		ret = true
	end
	return ret
end

function M:MouseMoved(x, y)
	local ret = false
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

function M:GetChecked()
	return self.checked
end

function M:Touch(x, y)
	local worldwidth = 10
	local width = self.width + (#self.text) * worldwidth
	return gui.math.PosInRect(x,y,self.x,self.y,width,self.height)
end

return M