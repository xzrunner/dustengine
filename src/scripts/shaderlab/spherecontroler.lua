-- local quaternion = require "ejoy3d.quaternion"
-- local render = require "ejoy3d.render.c"

local M = {}

local WIDTH = 800
local HEIGHT = 600

-- local orientation = quaternion.new({0,0,0,1})
-- render.set_orientation(orientation)
local sx, sy
local previous
local rotating = false	

local function maptosphere(x, y)
	local width = WIDTH
	local height = HEIGHT

	local trackball_radius = width / 3
	local centerx = width * 0.5
	local centery = height * 0.5

	local dx = x - centerx
	local dy = y - centery
	dy = -dy

	local radius = trackball_radius
	local safe_radius = radius - 1
	if dx*dx+dy*dy > safe_radius*safe_radius then
		local theta = math.atan2(dy, dx)
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
	previous = orientation			
end

function M.mousereleased(x, y)
	rotating = false
end

function M.mousemoved(x, y)
	if rotating then
		local s = maptosphere(sx, sy)
		local e = maptosphere(x, y)
		-- local delta = quaternion.create_from_vector(s, e)
		-- orientation = quaternion.rotated(previous, delta)
		-- render.set_orientation(orientation)			
	end	
end

return M