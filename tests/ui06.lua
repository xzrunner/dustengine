-- slider

local dust = require "dust"
local base = require "base"

local game = {}

function game.load()
	base.load()
	
	require "dust.gui"

	local frame = gui.create("frame")
	frame:SetTitle("Slider"):SetSize(300, 275):SetPos(150, 200)

	local slider1 = gui.create("slider", frame)
	slider1:SetPos(5, 30):SetWidth(290):SetMinMax(0, 100)
		
	local slider2 = gui.create("slider", frame)
	slider2:SetPos(5, 60):SetHeight(200):SetMinMax(0, 100)
	slider2:SetSlideType("vertical"):CenterX()
end

function game.update()
	base.update()
end

function game.draw()
	base.draw()
end

function game.touch(what, x, y)
	base.touch(what, x, y)
end

function game.key(what, key)
	base.key(what, key)
end

function game.message(id, state, sval, nval)
	base.message(id, state, sval, nval)
end

dust.start(game)