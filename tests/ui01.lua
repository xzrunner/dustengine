local dust = require "dust"
local gp = require "ejoy2d.graphics.c"

local game = {}

function game.load()
end

function game.update()
end

function game.draw()
	gp.set_color(255, 255, 0)
	gp.rectangle(true, 20, 10, 200, 100)
	gp.rectangle(false, 100, 200, 50, 50)

	gp.set_color(128, 0, 255)
	local line = {0,0, 100,100, 200,400, 400,200, 500}
	gp.line(line)

	local r,g,b,a = gp.get_color()
	gp.set_color(r*1.5,g,b)
	gp.point(400, 400)

	gp.triangle(false, 400,0, 600,100, -100,200)

	gp.set_color(r,g,b/2)
	local tri = {300,200, 500,500, 100,500}
	gp.triangle(true, tri)

	gp.circle(true, 400, 300, 50)
	gp.circle(false, 200, 100, 30, 5)
end

function game.touch(what, x, y)
end

function game.key(what, key)
	if what == "DOWN" then
	elseif what == "UP" then
	end
end

dust.start(game)