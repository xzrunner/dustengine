-- panel

local dust = require "dust"
local base = require "base"

local game = {}

function game.load()
	base.load()
	
	require "dust.gui"

	local frame = gui.create("frame")
	frame:SetTitle("Panel"):SetSize(210, 85):SetPos(150, 200)

	local panel = gui.create("panel", frame)
	panel:SetPos(5, 30)
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