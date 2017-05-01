local base = require "dust.gui.widgets.base"

local M = setmetatable({}, base)
M.__index = M

function M:initialize()
	base.initialize(self)

	self.type = "panel"
	self.width = 200
	self.height = 50
	self.children = {}
end

function M:Update()
	if not self.visible then return end

	for k, v in ipairs(self.children) do
		v:Update()
	end	
	
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
		drawfunc = skin.DrawPanel
	end
	drawfunc(self)

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
	for k, v in ipairs(self.children) do
		local skip = v:KeyDown(key)
		if skip then return true end
	end	
	return false
end

function M:KeyUp(key)
	for k, v in ipairs(self.children) do
		local skip = v:KeyUp(key)
		if skip then return true end
	end		
	return false
end

function M:Refresh()
	base.Refresh(self)

	for k, v in ipairs(self.children) do
		v:Refresh()
	end	
end

return M