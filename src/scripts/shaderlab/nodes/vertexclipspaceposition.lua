require "gui"
local base = require "shaderlab.nodes.base"
local interface = require "shaderlab.nodes.interface"
local shader = require "shaderlab.shader"
local util = require "shaderlab.util"

local M = setmetatable({}, base)
M.__index = M

local function _init_static(self)
	local attr = {}
	attr.type = 'vec4'
	attr.name = 'a_position'
	table.insert(self.attributes, attr)

	local uniform = {}
	uniform.type = 'mat4'
	uniform.name = 'u_projection'
	table.insert(self.uniforms, uniform)

	local uniform = {}
	uniform.type = 'mat4'
	uniform.name = 'u_modelview'
	table.insert(self.uniforms, uniform)	
end

function M:initialize(x, y)
	base.initialize(self)

	self.type = "vertexclipspaceposition"

	self.name = "static"
	self.datatype = "vec4"

	self.attributes = {}
	self.uniforms = {}
	_init_static(self)

	local frame = gui.create("frame")
	self.output = interface.create(frame, self, "out", "")
	frame:SetTitle("Vertex Clipspace Position"):SetSize(260, 25):SetPos(x, y)
	self.frame = frame
end

-- function M:Connect(interface)
-- 	base.Connect(self, interface)

-- 	local node = interface:GetNode()
-- 	local type = node:GetType()
-- 	if type == "outputposition" then
-- 		if self.shader then
-- 			shader.remove(true, self.shader.id)
-- 		end

-- 		local sentence = node:ToCode() .. " = " .. self:ToCode() .. ";"
-- 		self.shader = shader.insert(true, sentence)
-- 		shader.insert_node(self)
-- 		shader.build()
-- 	end
-- end

-- function M:Deconnect(interface)
-- 	if not self.shader then return end

-- 	for k, v in ipairs(self.output.connected) do
-- 		if v == interface then
-- 			local node = v:GetNode()
-- 			if node:GetType() == "outputposition" then
-- 				table.remove(self.output.connected, k)
-- 				util.table_remove_value(interface.connected, self.output)
-- 				shader.remove(true, self.shader.id)

-- 				local sentence = node:ToCode() .. " = vec4(0,0,0,0);"
-- 				self.shader = shader.insert(true, sentence)
-- 				shader.remove_node(self)
-- 				shader.build()
-- 				break
-- 			end
-- 		end
-- 	end
-- end

function M:ToCode()
	return "u_projection * u_modelview * a_position"
end

function M:ShaderInit()
end

return M