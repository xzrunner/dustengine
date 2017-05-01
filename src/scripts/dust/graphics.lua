local c = require "sprite2.c"
local dust = require "dust"

local graphics = {}

function graphics.set_color(...)
	c.rvg_set_color(...)
end

function graphics.set_linewidth(...)
	c.rvg_set_line_width(...)
end

local function _trans_proj(x, y)
    local hw = dust.screen_width * dust.screen_scale * 0.5
    local hh = dust.screen_height * dust.screen_scale * 0.5
    local x = x - hw
    local y = -(y - hh)
    return x, y
end

function graphics.line(x1, y1, x2, y2)
	x1, y1 = _trans_proj(x1, y1)
	x2, y2 = _trans_proj(x2, y2)
	c.rvg_draw_line(x1, y1, x2, y2)
end

function graphics.draw_rect(filling, x, y, w, h)
	x, y = _trans_proj(x, y)
	y = y - h
	c.rvg_draw_rect(filling, x, y, w, h)
end

function graphics.circle(filling, cx, cy, raidius, segments)
	cx, cy = _trans_proj(cx, cy)
	c.rvg_draw_circle(filling, cx, cy, raidius, segments)
end

return graphics