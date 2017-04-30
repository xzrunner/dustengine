local c = require "sprite2.c"

local graphics = {}

function graphics.set_color(...)
	c.rvg_set_color(...)
end

function graphics.draw_rect(...)
	c.rvg_draw_rect(...)
end

return graphics