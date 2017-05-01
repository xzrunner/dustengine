local base = require "dust.gui.widgets.base"

local M = setmetatable({}, base)
M.__index = M

local TITLE_HEIGHT = 25

function M:initialize()
	base.initialize(self)

	self.type = "frame"
	self.title = "Frame"
	self.width = 300
	self.height = 150
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
		drawfunc = skin.DrawFrame
	end
	drawfunc(self)

	for k, v in ipairs(self.children) do
		v:Draw()
	end
end

function M:MousePressed(x, y)
	local ret = false
	if not self.visible then return ret end

	self.selected = gui.math.PosInRect(x,y,self.x,self.y,self.width,TITLE_HEIGHT)

	if self.selected then
		self.xlast = x
		self.ylast = y
		return true
	end

	for k, v in ipairs(self.children) do
		local skip = v:MousePressed(x, y)
		if skip then return true end
	end
	return ret
end

function M:MouseReleased(x, y)
	local ret = false
	if not self.visible then return ret end

	self.selected = nil

	for k, v in ipairs(self.children) do
		local skip = v:MouseReleased(x, y)
		if skip then return true end
	end
	return ret
end

function M:MouseMoved(x, y)
	local ret = false
	if not self.visible then return ret end
	
	if self.selected then
		local dx = x - self.xlast
		local dy = y - self.ylast		
		self.x = self.x + dx
		self.y = self.y + dy
		self.xlast = x
		self.ylast = y
		self:Refresh()
		return true
	end

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

function M:SetTitle(title)
	self.title = title
	self:Refresh()
	return self
end

function M:GetTitle()
	return self.title
end

return M