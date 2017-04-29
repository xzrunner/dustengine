local sprc = require "sprite2.c"
local gumc = require "gum.c"

local M = {}

local mt = {}
mt.__index = mt

function M.create_screen_rt()
    local self = {
        v_impl = gumc.rt_fetch(),
        v_type = "SCREEN"
    }
    assert(self.v_impl)    
    return setmetatable(self, mt)
end

function M.create_tmp_rt()
    local self = {
        v_impl = sprc.rt_fetch(),
        v_type = "TMP"
    }
    assert(self.v_impl)
    return setmetatable(self, mt)
end

local SCREEN_RT = nil

function M.set_screen_rt(rt)
    SCREEN_RT = rt
end

function M.get_screen_rt()
    return SCREEN_RT
end

function mt:__gc()
    self:release()
end

function mt:release()
    if self.v_type == "SCREEN" then
        gumc.rt_return(self.v_impl)
    elseif self.v_type == "TMP" then
        sprc.rt_return(self.v_impl)
    end
end

function mt:bind()
    if self.v_type == "SCREEN" then
        gumc.rt_bind(self.v_impl)
    end
end

function mt:unbind()
    if self.v_type == "SCREEN" then
        gumc.rt_unbind(self.v_impl)
    end
end

function mt:draw(r_src)
    if self.v_type == "SCREEN" then
        gumc.rt_draw(self.v_impl, r_src)
    end
end

function mt:draw_to(r_src, r_dst)
    if self.v_type == "TMP" then
        sprc.rt_draw_to(self.v_impl, r_src, r_dst)
    end
end

function mt:draw_from(r_src, r_dst, rt_src)
    if self.v_type == "TMP" then
        local tex_id = rt_src:get_tex_id()
        if tex_id then
            sprc.rt_draw_from(self.v_impl, r_src, r_dst, tex_id)
        end
    end
end

function mt:get_tex_id()
    local tex_id = nil
    if self.v_type == "SCREEN" then        
        tex_id = gumc.rt_get_texid(self.v_impl)
    elseif self.v_type == "TMP" then
        tex_id = sprc.rt_get_texid(self.v_impl)
    end
    return tex_id
end

return M