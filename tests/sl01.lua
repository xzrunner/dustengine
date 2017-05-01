-- simple shader

local dust = require "dust"
local render = require "ejoy3d.render.c"
require "shaderlab"

local game = {}

function game.load()
	render.load_mesh("cone", 2, 1)
	-- render.load_mesh("sphere", 1)

	local vertex_pos = shaderlab.create("vertexclipspaceposition", 200, 100)
	local output_pos = shaderlab.create("outputposition", 600, 100)
	local rgbacolor = shaderlab.create("rgbacolor", 200, 300)
	local outputcolor = shaderlab.create("outputcolor", 600, 300)
	vertex_pos:Connect(output_pos:GetInputNode(1))
	rgbacolor:Connect(outputcolor:GetInputNode(1))	
end

function game.update()
	shaderlab.update()
end

function game.draw()
	shaderlab.draw()
end

function game.touch(what, x, y)
	if what == "BEGIN" then
		shaderlab.mousepressed(x, y)	
	elseif what == "END" then
		shaderlab.mousereleased(x, y)	
	elseif what == "RIGHT_BEGIN" then
		shaderlab.rightmousepressed(x, y)
	elseif what == "RIGHT_END" then
		shaderlab.rightmousereleased(x, y)
	elseif what == "MOVE" then
		shaderlab.mousemoved(x, y)
	end
end

function game.key(what, key)
	if what == "DOWN" then
	elseif what == "UP" then
	end
end

dust.start(game)


