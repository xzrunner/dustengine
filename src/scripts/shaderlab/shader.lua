-- local c = require "ejoy3d.render.c"
local util = require "shaderlab.util"

local PRECISION = ""

local ID = 0

local attributes = {}
attributes[1] = "attribute vec4 a_position;"

local vs_uniforms = {}
vs_uniforms[1] = "uniform mat4 u_projection;"
vs_uniforms[2] = "uniform mat4 u_modelview;"

local fs_uniforms = {}

local shader = {}

local VS = {}
local FS = {}

local NODES = {}

function shader.build()
	-- -- vs
	-- local vs = ""
	-- for k, v in ipairs(attributes) do
	-- 	vs = vs .. v
	-- end
	-- for k, v in ipairs(vs_uniforms) do
	-- 	vs = vs .. v
	-- end
	-- vs = vs .. "void main(void) {"
	-- for k, v in ipairs(VS) do
	-- 	vs = vs .. v.sentence
	-- end
	-- vs = vs .. "}"

	-- -- fs
	-- local fs = ""	
	-- for k, v in ipairs(fs_uniforms) do
	-- 	fs = fs .. v
	-- end	
	-- fs = fs .. "void main(void) {"
	-- for k, v in ipairs(FS) do
	-- 	fs = fs .. v.sentence
	-- end
	-- fs = fs .. "}"

	-- print("vs: " .. vs)
	-- print("fs: " .. fs)

	-- c.load_shader(vs, fs)
	-- for _, v in ipairs(NODES) do
	-- 	v:ShaderInit()
	-- end
end

function shader.insert(vs, sentence)
	local t = vs and VS or FS

	local shader = {}
	shader.id = ID
	ID = ID + 1
	shader.sentence = sentence
	table.insert(t, shader)
	return shader
end

function shader.remove(vs, id)
	local t = vs and VS or FS

	for k, v in ipairs(t) do
		if v.id == id then
			table.remove(t, k)
			break
		end
	end
end

function shader.insert_uniform(vs, type, name)
	local t = vs and vs_uniforms or fs_uniforms
	local uniform = "uniform" .. " " .. type .. " " .. name .. ";"
	for k, v in pairs(t) do
		if v == uniform then return end
	end
	table.insert(t, uniform)
end

function shader.insert_node(node)
	for _, v in ipairs(NODES) do
		if v == node then return end
	end
	table.insert(NODES, node)
end

function shader.remove_node(node)
	util.table_remove_value(NODES, node)
end

return shader