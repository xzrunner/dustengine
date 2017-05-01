local M = {}
M.__index = M

function M:initialize()
	self.x = 0
	self.y = 0
	self.width = 1024
	self.height = 768
	self.visible = true
end

function M:SetPos(x, y)
	if self.parent then
		self.ox = x
		self.oy = y
	else
		self.x = x
		self.y = y
	end
	self:Refresh()
	return self
end

function M:GetX()
	return self.x
end

function M:GetY()
	return self.y
end

function M:GetPos()
	return self.x, self.y
end

function M:SetSize(w, h)
	self.width = w
	self.height = h
	self:Refresh()
	return self
end

function M:SetWidth(w)
	self.width = w
	self:Refresh()
	return self
end

function M:SetHeight(h)
	self.height = h
	self:Refresh()
	return self
end

function M:GetWidth()
	return self.width
end

function M:GetHeight()
	return self.height
end

function M:GetSize()
	return self.width, self.height
end

function M:Center()
	local parent = self.parent
	if parent then
		self.ox = parent.width/2 - self.width/2
		self.oy = parent.height/2 - self.height/2		
	else
		self.x = parent.width/2 - self.width/2
		self.y = parent.height/2 - self.height/2	
	end
	self:Refresh()
	return self
end

function M:CenterX()
	local parent = self.parent	
	if parent then
		self.ox = parent.width/2 - self.width/2
	else
		self.x = parent.width/2 - self.width/2
	end
	self:Refresh()
	return self
end

function M:CenterY()
	local parent = self.parent	
	if parent then
		self.oy = parent.height/2 - self.height/2
	else
		self.y = parent.height/2 - self.height/2
	end
	self:Refresh()
	return self
end

function M:SetVisible(visible)	
	self.visible = visible

	local children = self.children
	if children then
		for k, v in ipairs(children) do
			v:SetVisible(visible)
		end
	end
	
	return self
end

function M:GetVisible()
	return self.visible	
end

function M:SetParent(parent)
	self:Remove()
	self.parent = parent
	self.ox = 0
	self.oy = 0
	table.insert(parent.children, self)
	return self
end

function M:Remove()
	if not self.parent then return end

	local pchildren = self.parent.children
	if pchildren then
		for k, v in ipairs(pchildren) do
			if v == self then
				table.remove(pchildren, k)
			end
		end
	end
	return self
end

function M:GetSkin()
	local skins = gui.skins
	local self_index = self.skin
	local default_index = gui.config["DEFAULTSKIN"]
	local skin = skins[self_index] or skins[default_index]
	return skin
end

function M:Refresh()
	if not self.ox or not self.oy then return end

	local parent = self.parent
	if parent then
		self.x = parent.x + self.ox
		self.y = parent.y + self.oy
	end	
end

return M