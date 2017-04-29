local dust = require "dust"
local Sprite = require "dust.sprite"
local ShaderLab = require 'dust.shaderlab'

local game = {}

local sprites = {}

function game.load()
    local spr = Sprite.new_spr_from_file('tests/asset/raw/icon_flag_win.png')
    assert(spr)
    table.insert(sprites, spr)
end

local x = 0
function game.update(delta_time)
	x = x + 1
	for i = 1, #sprites do
		sprites[i]:ps(x, 0)
	end
end

function game.draw(delta_time)
    ShaderLab.clear()

	for i = 1, #sprites do
		sprites[i]:draw_ui()
	end
end

dust.start(game)