-- checkbox

local dust = require "dust"
local base = require "base"

local game = {}

function game.load()
	base.load()
	
	require "dust.gui"

	local frame = gui.create("frame")
	frame:SetTitle("Checkbox"):SetHeight(85):SetPos(150, 200)

	local checkbox1 = gui.create("checkbox", frame)
	checkbox1:SetText("Checkbox 1"):SetPos(5, 30)

	local checkbox2 = gui.create("checkbox", frame)
	checkbox2:SetText("Checkbox 2"):SetPos(5, 60)
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