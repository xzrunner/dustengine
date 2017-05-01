local Graphics = require "dust.graphics"
local parser = require "shaderlab.grammarparser"

local M = {}
M.__index = M

local LINE_COLOR    = { 83, 255, 255, 255}
local LINE_WIDTH    = 3

function M:initialize()
	self.type = ""
	self.frame = nil
	self.input = {}
	self.output = nil
end

function M:Update()
end

function M:Draw()
	self.frame:Draw()

	for k, v in ipairs(self.input) do
		v:Draw()
	end

	print("base draw 0")
	if self.output then
	print("base draw 1")		
		self.output:Draw()

		for _,v in ipairs(self.output.connected) do
	print("base draw 2")			
			local x1, y1 = self:GetOutputPos()
			local x2, y2 = v:GetPos()
			Graphics.set_color(LINE_COLOR)
			Graphics.set_linewidth(LINE_WIDTH)
			Graphics.line(x1, y1, x2, y2)
		end		
	end
end

function M:TouchOutput(x, y)
	if not self.output then return false end
	return self.output:Touch(x, y)
end

function M:TouchInput(x, y)
	for k, v in ipairs(self.input) do
		if v:Touch(x, y) then
			return v
		end
	end
	return nil
end

function M:GetInputNode(idx)
	return self.input[idx]
end

function M:GetOutputPos()
	if self.output then
		return self.output:GetPos()
	end
end

function M:SetName(name)
	self.name = name
	return self
end

function M:GetName()
	return self.name
end

function M:GetDataType()
	return self.datatype
end

function M:OnChange()
	-- do something

	for _,v in self.output.connected do
		v:OnChange()
	end
end

function M:GetType()
	return self.type
end

function M:Connect(interface)
	self.output:Connect(interface)
	interface:GetNode():Connected()

	print("\n[connect] " .. self.name .. " to " .. interface:GetNode():GetName())
	parser.print()
end

function M:Deconnect(interface)
	self.output:Deconnect(interface)
	interface:GetNode():Deconnected()

	print("\n[deconnect] " .. self.name .. " from " .. interface:GetNode():GetName())
	parser.print()
end

function M:Connected()
end

function M:Deconnected()
end

return M