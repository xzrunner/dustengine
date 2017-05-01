-- vector = require "libraries.vector"

local dust = require "dust"
local render = require "ejoy3d.render.c"
local bezier = require "shaderlab.bezier"
require "shaderlab"

local game = {}

function game.load()

	-- local bezier_test = setmetatable({}, bezier)
	-- bezier_test:Init(vector(0, 0), vector(100, 100))

	render.load_mesh("cone", 2, 1)
	-- render.load_mesh("sphere", 1)

	require "dust.gui"

	local frame = gui.create("frame")
	frame:SetTitle("Color Changer"):SetSize(500, 255):SetPos(150, 200)

	local colorchanger = gui.create("colorchanger", frame)
	colorchanger:SetPos(0, 0)
	colorchanger.OnColorChanged = function(color)
		render.set_mesh_diffuse_color(color)
	end
end

function game.update()
	gui.update()
end

function game.draw()
	gui.draw()
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


