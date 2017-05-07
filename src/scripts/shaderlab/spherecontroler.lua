local Camera3 = require "dust.camera3"

local M = {}

local WIDTH = 800
local HEIGHT = 600

local sx, sy
local rotating = false	

local cam = Camera3()
cam:translate(0, 0, -3)

local function maptosphere(x, y)
	local Gum = require "dust.gum"
	local width, height = Gum.get_screen_size()

	local trackball_radius = width / 3
	local centerx = width * 0.5
	local centery = height * 0.5

	local dx = x - centerx
	local dy = y - centery
	dy = -dy

	local radius = trackball_radius
	local safe_radius = radius - 1
	if dx*dx+dy*dy > safe_radius*safe_radius then
		local theta = math.atan(dy, dx)
		dx = safe_radius * math.cos(theta)
		dy = safe_radius * math.sin(theta)
	end

	local z = math.sqrt(radius*radius - dx*dx - dy*dy)
	local pos = {dx/radius, dy/radius, z/radius}
	return pos
end

function M.mousepressed(x, y)
	rotating = true
	sx = x
	sy = y
end

function M.mousereleased(x, y)
	rotating = false
end

function M.mousemoved(x, y)
	if rotating then
		local s = maptosphere(sx, sy)
		local e = maptosphere(x, y)	
		cam:rotate(s, e)
		cam:bind()
		sx = x
		sy = y
	end	
end

return M