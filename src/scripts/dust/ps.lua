local c = require "sprite2.c"

local ps = {}

function ps.buffer_draw(...)
	c.p3d_buffer_draw(...)
end

return ps