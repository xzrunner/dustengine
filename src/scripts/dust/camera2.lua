local c = require "sprite2.c"

local mt = {}
mt.__index = mt

local function new()
	local self = {
		v_impl = c.cam_create(),
		v_region = {}
	}
	return setmetatable(self, mt)
end

function mt:__gc()
    c.cam_release(self.v_impl)
end

function mt:bind()
	c.cam_bind(self.v_impl)
end

function mt:set(x, y, scale)
	c.cam_set(self.v_impl, x, y, scale)
end

function mt:get_impl()
	return self.v_impl
end

function mt:screen2project(x, y)
	return c.cam_screen2project(self.v_impl, x, y)
end

return new