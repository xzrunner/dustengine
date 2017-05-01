-- text input

local dust = require "dust"
local game = {}

function game.load()
	require "dust.gui"

	local frame = gui.create("frame")
	frame:SetTitle("Input"):SetSize(200, 100):SetPos(150, 200)

	local input1 = gui.create("input", frame)
	input1:SetPos(5, 30):SetText("string"):SetType("string")

	local input2 = gui.create("input", frame)
	input2:SetPos(5, 60):SetText("number"):SetType("number")
end

function game.update()
	gui.update()
end

function game.draw()
	gui.draw()
end

function game.touch(what, x, y)
	if what == "BEGIN" then
		gui.mousepressed(x, y)
	elseif what == "END" then
		gui.mousereleased(x, y)
	elseif what == "MOVE" then
		gui.mousemoved(x, y)
	end
end

function game.key(what, key)
	if what == "DOWN" then
		gui.keydown(key)
	elseif what == "UP" then
		gui.keyup(key)
	end
end

dust.start(game)