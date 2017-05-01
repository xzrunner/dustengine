local base = require "dust.gui.widgets.base"
local style = require "dust.gui.style"

local M = setmetatable({}, base)
M.__index = M

function M:initialize()
	base.initialize(self)

	self.type = "treelist"
	self.width = 150
	self.height = 300
	self.nodes = {}

	self.selected = nil
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
		drawfunc = skin.DrawTreelist
	end
	drawfunc(self)
end

function M:MousePressed(x, y)
	local ret = false
	if not self.visible then return ret end

	local n = self:FindNodeByPos(x, y)
	if n then
		n.expand = not n.expand

		local index = (y - self.y - style.treelist_top_offset) / style.treelist_node_height
		self.selected = {}
		self.selected.name = n.name
		self.selected.pos = math.floor(index)
	else
		self.selected = nil
	end

	return ret
end

function M:MouseReleased(x, y)
	local ret = false
	if not self.visible then return ret end
	self.selected = nil
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

function M:GetSelectedPos()
	if self.selected then
		return self.selected.pos
	else
		return -1
	end
end

function M:GetSelectedName()
	if self.selected then
		return self.selected.name
	else
		return nil
	end	
end

function M:GetNodes()
	return self.nodes
end

function M:AddNode(name, parent)
	local node = {}
	node.name = name
	node.expand = false
	if parent and parent ~= "" then
		local p = self:FindNode(self.nodes, parent)
		if p then
			if not p.nodes then
				p.nodes = {}
			end
			table.insert(p.nodes, node)
		end
	else
		table.insert(self.nodes, node)
	end
	return self
end

function M:FindNode(nodes, name)
	for k, v in ipairs(nodes) do
		if v.name == name then
			return v
		end
		if v.nodes then
			local n = self:FindNode(v.nodes, name)
			if n then return n end
		end
	end
	return nil
end

local currx
local function _findnode(y, nodes)
	local height = style.treelist_node_height
	for k, v in ipairs(nodes) do
		if y > curry and y < curry + height then
			return v
		end
		curry = curry + height
		if v.nodes and v.expand then
			local n = _findnode(y, v.nodes)
			if n then return n end
		end
	end
	return nil
end

function M:FindNodeByPos(x, y)
	if x < self.x or x > self.x + self.width or 
	   y < self.y or y > self.y + self.height then
		return nil
	end
	curry = self.y + style.treelist_top_offset
	return _findnode(y, self.nodes)
end

return M