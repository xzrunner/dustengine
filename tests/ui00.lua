local dust = require "dust"
local fw = require "ejoy2d.framework"
local pack = require "ejoy2d.simplepackage"
local g = require "ejoy2d.graphics.c"

pack.load {
	pattern = fw.WorkDir..[[examples/asset/?]],
	"sample",
}

local obj = ej.sprite("sample","cannon")
local turret = obj.turret
-- set position (-100,0) scale (0.5)
obj:ps(-100,0,0.5)

local obj2 = ej.sprite("sample","mine")
obj2.resource.frame = 70
-- set position(100,0) scale(1.2) separately
obj2:ps(100,0)
obj2:ps(1.2)

local game = {}
local screencoord = { x = 512, y = 384, scale = 1.2 }
local x1,y1,x2,y2 = obj2:aabb(screencoord)
obj2.label.text = string.format("AABB\n%d x %d", x2-x1, y2-y1)

function game.update()
	turret.frame = turret.frame + 3
	obj2.frame = obj2.frame + 1
end

function game.draw()
	
	obj:draw(screencoord)


g.set_color(255, 255, 0)
g.rectangle(false, 400, 300, 200, 100)
-- g.point(math.random(0,100), math.random(0,100))

-- local line = {0,0, 100,100, 200,400, 400,200, 500}
-- g.line(line)

g.set_color(255, 0, 255)
g.triangle(false, 0,0, 100,100, -100,200)

-- local tri = {300,200, 100,100, 100,500}
-- g.triangle(true, tri)

g.circle(true, 400, 300, 50)
g.circle(false, 200, 100, 30, 5)

	obj2:draw(screencoord)

	-- g.rectangle(true, 0, 0, 500, 200)
end

function game.touch(what, x, y)
end

dust.start(game)


