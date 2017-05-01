-- button

local dust = require "dust"
local base = require "base"

local game = {}

function game.load()
	base.load()

	require "dust.gui"

	local frame = gui.create("frame")
	frame:SetPos(150, 200)
	-- frame:Center()

	local button = gui.create("button", frame)
	button:SetWidth(180):Center()
	button.OnClick = function(object)
		object:SetText("clicked!")
	end
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