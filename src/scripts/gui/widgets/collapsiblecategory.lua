local base = require "gui.widgets.base"

local M = setmetatable({}, base)
M.__index = M

function M:initialize()
	base.initialize(self)

	self.type = "collapsiblecategory"
	self.title = "Category"
	self.width = 200
	self.height = 25
	self.closedheight = 25
	self.padding = 5
	self.open = true
	self.children = {}
end

function M:Update()
	if not self.visible then return end

	local update = self.UpdateCB
	if update then
		update(self)
	end

	local open = self.open
	local child = self.children[1]
	if open and child then
		child:Update()
	end
end

function M:Draw()
	if not self.visible then return end

	local drawfunc = self.DrawCB
	if not drawfunc then
		local skin = self:GetSkin()
		drawfunc = skin.DrawCollapsibleCategory
	end
	drawfunc(self)	

	local open = self.open
	local child = self.children[1]
	if open and child then
		child:Draw()
	end
end

function M:MousePressed(x, y)
	local ret = false
	if not self.visible then return ret end

	if self:Touch(x, y) then
		self:ChangeOpen()
		ret = true
	end

	if ret then return true end

	local open = self.open
	local child = self.children[1]
	if open and child then
		ret = child:MousePressed(x, y)
	end
	return ret
end

function M:MouseReleased(x, y)
	local ret = false
	if not self.visible then return ret end

	local open = self.open
	local child = self.children[1]
	if open and child then
		ret = child:MouseReleased(x, y)
	end
	return ret
end

function M:MouseMoved(x, y)
	local ret = false
	if not self.visible then return ret end

	local open = self.open
	local child = self.children[1]
	if open and child then
		ret = child:MouseMoved(x, y)
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

	local open = self.open
	local child = self.children[1]
	if open and child then
		child:Refresh()
	end
end

function M:SetChild(child)
	local curr = self.children[1]
	if curr then
		curr:Remove()
		self.children = {}
	end

	child:Remove()
	child.parent = self
	child:SetWidth(self.width - self.padding*2)
	child:SetPos(self.padding, self.closedheight + self.padding)
	table.insert(self.children, child)
end

function M:SetText(text)
	self.text = text
	return self
end

function M:GetText(text)
	return self.text
end

function M:ChangeOpen()
	self.open = not self.open
	local child = self.children[1]
	if child then
		child:SetVisible(self.open)
	end
end

function M:IsOpen()
	return self.open
end

function M:SetTitle(title)
	self.title = title
	self:Refresh()
	return self
end

function M:GetTitle()
	return self.title
end

function M:GetClosedHeight()
	return self.closedheight
end

function M:Touch(x, y)
	-- local edge = self.closedheight
	-- return gui.math.PosInRect(x,y,self.x+self.width-edge,self.y,edge,edge)
	return gui.math.PosInRect(x,y,self.x,self.y,self.width,self.closedheight)
end

return M