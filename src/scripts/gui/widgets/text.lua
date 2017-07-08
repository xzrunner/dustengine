local base = require "gui.widgets.base"

local M = setmetatable({}, base)
M.__index = M

function M:initialize()
	base.initialize(self)

	self.type = "text"
	self.width = 5
	self.height = 5
	self.text = ""
	self.font = 14
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
		drawfunc = skin.DrawText
	end
	drawfunc(self)
end

function M:MousePressed(x, y)
	local ret = false
	if not self.visible then return ret end
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
	return false
end

function M:KeyUp(key)
	return false
end

function M:SetText(text)
	self.text = text
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

return M