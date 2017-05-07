local c = require "model3.c"
local ShaderLab = require 'dust.shaderlab'

local mt = {}
mt.__index = mt

local function new()
	local self = {
		v_impl = c.cam_create(),
		v_region = {},
		v_mat = {},
	}
	return setmetatable(self, mt)
end

function mt:__gc()
    c.cam_release(self.v_impl)
end

function mt:bind()
	c.cam_get_mat(self.v_impl, self.v_mat)
	ShaderLab.set_modelview3(self.v_mat)
end

function mt:rotate(begin, _end)
	c.cam_rotate(self.v_impl, begin, _end)
end

function mt:translate(x, y, z)
	c.cam_translate(self.v_impl, x, y, z)	
end

return new