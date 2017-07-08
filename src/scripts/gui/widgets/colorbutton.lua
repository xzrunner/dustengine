local base = require "gui.widgets.base"

local M = setmetatable({}, base)
M.__index = M

function M:initialize()
	base.initialize(self)

	self.type = "colorbutton"
	self.color = {128, 128, 0}
	self.width = 20
	self.height = 20
	self.OnColorChanged	= nil

	local frame = gui.create("frame")
	frame:SetTitle("Color Changer"):SetSize(500, 255):SetPos(150, 200)
	local colorchanger = gui.create("colorchanger", frame)
	colorchanger:SetPos(0, 0)	
	colorchanger.OnColorChanged = function(color)
		self.color = color
		local oncolorchanged = self.OnColorChanged
		if oncolorchanged then
			oncolorchanged(self.color)
		end
	end	
	frame:SetVisible(false)
	self.colorframe = frame
	self.colorchanger = colorchanger
end

function M:Update()
	if not self.visible then return end
end

function M:Draw()
	if not self.visible then return end

	local drawfunc = self.DrawCB
	if not drawfunc then
		local skin = self:GetSkin()
		drawfunc = skin.DrawColorbutton
	end
	drawfunc(self)	

	self.colorframe:Draw()
end

function M:MousePressed(x, y)
	local ret = false
	if not self.visible then return end

	local colorframe = self.colorframe
	local color_visible = colorframe:GetVisible()
	if self:TouchButton(x, y) then
		colorframe:SetVisible(not color_visible)
		ret = true
	elseif color_visible and not self:TouchColorchanger(x, y) then
		colorframe:SetVisible(false)
		ret = true
	end	

	return ret
end

function M:MouseReleased(x, y)
	if not self.visible then return end
end

function M:MouseMoved(x, y)
	if not self.visible then return end
end

function M:KeyDown(key)
	return false
end

function M:KeyUp(key)
	return false
end

function M:GetColor()
	local color = self.color
	return self.color
end

function M:SetColor(color)
	self.color = color
	self.colorchanger:SetColor(color)
	return self
end

function M:TouchButton(x, y)
	return gui.math.PosInRect(x,y,self.x,self.y,self.width,self.height)
end

function M:TouchColorchanger(x, y)
	local _x, _y = self.colorframe:GetPos()
	local w, h = self.colorframe:GetSize();
	return gui.math.PosInRect(x,y,_x,_y,w,h)	
end

return M