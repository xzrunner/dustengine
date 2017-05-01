require "dust.gui"
-- local render = require "ejoy3d.render.c"
local edit = require "shaderlab.connectedit"
local ctrl = require "shaderlab.spherecontroler"
local frame = require "shaderlab.frame"

shaderlab = {}

shaderlab.nodes = {}

function shaderlab.register(container, path, name)
	local class = require ("shaderlab." .. path)
	container[name] = class
end

function shaderlab.load()
	shaderlab.nodes = {}
	shaderlab.register(shaderlab.nodes, "nodes.add", "add")
	shaderlab.register(shaderlab.nodes, "nodes.substract", "substract")
	shaderlab.register(shaderlab.nodes, "nodes.multiply", "multiply")	
	shaderlab.register(shaderlab.nodes, "nodes.outputposition", "outputposition")
	shaderlab.register(shaderlab.nodes, "nodes.outputcolor", "outputcolor")	
	shaderlab.register(shaderlab.nodes, "nodes.rgbacolor", "rgbacolor")		
	shaderlab.register(shaderlab.nodes, "nodes.vertexclipspaceposition", "vertexclipspaceposition")
	shaderlab.register(shaderlab.nodes, "nodes.vertexnormal", "vertexnormal")
	shaderlab.register(shaderlab.nodes, "nodes.vertexxyz", "vertexxyz")
	shaderlab.register(shaderlab.nodes, "nodes.localtoview", "localtoview")
	shaderlab.register(shaderlab.nodes, "nodes.multiply44", "multiply44")
	shaderlab.register(shaderlab.nodes, "nodes.float1", "float1")
	shaderlab.register(shaderlab.nodes, "nodes.float2", "float2")
	shaderlab.register(shaderlab.nodes, "nodes.float3", "float3")
	shaderlab.register(shaderlab.nodes, "nodes.float4", "float4")	
	shaderlab.register(shaderlab.nodes, "nodes.normalize", "normalize")		
	shaderlab.register(shaderlab.nodes, "nodes.max", "max")
	shaderlab.register(shaderlab.nodes, "nodes.dotproduct3", "dotproduct3")	
	shaderlab.register(shaderlab.nodes, "nodes.pow", "pow")		
end

function shaderlab.create(name, x, y)
	local class = shaderlab.nodes[name]
	if class then
		local obj = setmetatable({}, class)
		obj:initialize(x, y)
		table.insert(shaderlab.nodes, obj)
		return obj
	end
end

function shaderlab.update()
	gui.update()
	for k, v in ipairs(shaderlab.nodes) do
		v:Update()
	end	
end

function shaderlab.draw()
	-- gui.draw()
	for k, v in ipairs(shaderlab.nodes) do
		v:Draw()
	end

	edit.draw()
	frame.draw()
end

function shaderlab.mousepressed(x, y)
	local skip = edit.mousepressed(x, y)
	if skip then return end

	gui.mousepressed(x, y)
	if not gui.pressed(x, y) then
		ctrl.mousepressed(x, y)	
	end	
	frame.mousepressed(x, y)
end

function shaderlab.mousereleased(x, y)
	edit.mousereleased(x, y)
	gui.mousereleased(x, y)
	ctrl.mousereleased(x, y)
	frame.mousereleased(x, y)
end

function shaderlab.rightmousepressed(x, y)
	frame.rightmousepressed(x, y)

	shaderlab.tab_viewport = true
	shaderlab.xlast = x
	shaderlab.ylast = y
end

function shaderlab.rightmousereleased(x, y)
	frame.rightmousereleased(x, y)
	shaderlab.tab_viewport = false
end

function shaderlab.mousemoved(x, y)
	edit.mousemoved(x, y)
	gui.mousemoved(x, y)
	frame.mousemoved(x, y)

	if shaderlab.tab_viewport then
		local dx = x - shaderlab.xlast
		local dy = y - shaderlab.ylast;
		-- render.translate(dx, dy)
		shaderlab.xlast = x
		shaderlab.ylast = y
	else
		ctrl.mousemoved(x, y)
	end
end

function shaderlab.keydown(key)
	gui.keydown(key)
end

function shaderlab.keyup(key)
	gui.keyup(key)
end

shaderlab.load()