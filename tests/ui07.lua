-- colorchanger

local dust = require "dust"
local base = require "base"

local game = {}

function game.load()
	base.load()

	require "dust.gui"

	local frame = gui.create("frame")
	frame:SetTitle("Color Changer"):SetSize(500, 255):SetPos(150, 200)

	local colorchanger = gui.create("colorchanger", frame)
	colorchanger:SetPos(0, 0)
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