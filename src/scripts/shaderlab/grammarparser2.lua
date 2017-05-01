-- local c = require "ejoy3d.render.c"
local util = require "shaderlab.util"
local shader = require "shaderlab.shadergrammar"

local M = {}

local ASSIGN_NODES = {}

M.vert = setmetatable({}, shader)
M.vert:initialize()
M.frag = setmetatable({}, shader)
M.frag:initialize()

local function _refresh_shader()
	local vs = M.vert:Parser()
	local fs = M.frag:Parser()
	print("----------- vert shader -----------")
	print(vs)
	print("----------- frag shader -----------")
	print(fs)

	-- c.load_shader(vs, fs)
	-- for k, v in pairs(ASSIGN_NODES) do
	-- 	-- print(v.output.name)
	-- 	local node = v.output
	-- 	if node.ShaderInit then
	-- 		node:ShaderInit()
	-- 	end
	-- 	local node = v.input1
	-- 	if node.ShaderInit then
	-- 		node:ShaderInit()
	-- 	end
	-- 	local node = v.input2
	-- 	if node and node.ShaderInit then
	-- 		node:ShaderInit()
	-- 	end
	-- end
end

function M.add(output, input1, input2)
	local assign_node = {}
	assign_node.output = output
	assign_node.input1 = input1
	assign_node.input2 = input2
	table.insert(ASSIGN_NODES, assign_node)
	
	if output:GetName() == "gl_FragColor" then
		M.frag:Clear()
		M.parser(output, M.frag)
		_refresh_shader()
	elseif output:GetName() == "gl_Position" then
		M.vert:Clear()
		M.parser(output, M.vert)
		-- _refresh_shader()
	else
		-- M.vert:Clear()
		M.parser(output, M.vert)
		_refresh_shader()
	end
end

function M.remove(output)
	for k, v in pairs(ASSIGN_NODES) do
		if v.output == output then
			table.remove(ASSIGN_NODES, k)
			break
		end
	end
end

function M.find(output)
	for k, v in pairs(ASSIGN_NODES) do
		if v.output == output then
			return v
		end
	end
	return nil
end

function M.parser(output, shader)
	-- for attribute and uniform
	if output.name == "static" then
		if output.attributes then
			for k, v in pairs(output.attributes) do
				table.insert(shader.attributes, v)
			end
		end
		if output.uniforms then
			for k, v in pairs(output.uniforms) do
				table.insert(shader.uniforms, v)
			end
		end		
	end

	local assign_node = M.find(output)
	if assign_node then
		local middles = shader.middles
		local assign = output:ToCode()
		-- table.insert(middles, assign)
		middles[#middles+1] = assign
		if assign_node.input1 then
			-- print("parser1:"..assign_node.input1:GetName())
			M.parser(assign_node.input1, shader)
		end
		if assign_node.input2 then
			-- print("parser2:"..assign_node.input2:GetName())
			M.parser(assign_node.input2, shader)
		end
	else
		local name = output:GetName()
		if name ~= "static" then
			-- local uniform = {}
			-- uniform.name = output:GetName()
			-- uniform.type = output:GetDataType()
			-- print("add uniform 1:" .. uniform.name)
			-- table.insert(shader.uniforms, uniform)		

			if output.attributes then
				for k, v in ipairs(output.attributes) do
					table.insert(shader.attributes, v)
				end				
			end
			if output.uniforms then
				print("output.uniforms:" .. output.name .. ", size:" .. #output.uniforms)
				for k, v in ipairs(output.uniforms) do
					table.insert(shader.uniforms, v)
				end				
			end			
		end
	end
end

return M