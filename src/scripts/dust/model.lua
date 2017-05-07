local c = require "model3.c"
local Gum = require "dust.gum"
local s2 = require "sprite2.c"

local m3 = {}

function m3.create_model_from_surface(type, ...)
	local model = c.create_model_from_surface(type, ...)
	return Gum.create_sym_model(model)
end

function m3.create_model_from_file(filepath)
	local model = c.create_model_from_file(filepath)
	return Gum.create_sym_model(model)
end

function m3.draw_model(model, x, y)
	s2.method.draw_symbol2(model, x, y)
end

return m3