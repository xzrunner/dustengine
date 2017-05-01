local dust = require "dust"
local GTxt = require 'dust.gtxt'
local ShaderLab = require 'dust.shaderlab'

local base = {}

function base.load()
	-- init gtxt
    GTxt.create()
    local font_file = 'tests/asset/SourceHanSansCN-Medium.otf'
    GTxt.add_font('default', font_file)
end

function base.update()
	gui.update()
end

function base.draw()
	ShaderLab.clear()
	gui.draw()
end

function base.touch(what, x, y)
	if what == "BEGIN" then
		gui.mousepressed(x, y)
	elseif what == "END" then
		gui.mousereleased(x, y)
	elseif what == "MOVE" then
		gui.mousemoved(x, y)
	end	
end

function base.key(what, key)
	if what == "DOWN" then
	elseif what == "UP" then
	end
end

function base.message(id, state, sval, nval)
    if id == 0 then
        if state == 'RESIZE' then
            local w, h, s = string.match(sval, "(%d+);(%d+);([%d.]+)")
		    dust.screen_width  = w
		    dust.screen_height = h
		    dust.screen_scale  = s
        end
    end
end

return base