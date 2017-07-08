-- button

local dust = require "dust"
local base = require "base"

local editor = {}

function editor.load()
	base.load()

	require "gui"

	-- tween = require("third-party.tween")

	local frame = gui.create("frame")
	frame:SetPos(150, 200)
	-- frame:Center()

	local button = gui.create("button", frame)
	button:SetWidth(180):Center()
	button.OnClick = function(object)
		object:SetText("clicked!")
	end
end

function editor.update()
	base.update()

	-- tween.update(0.033)
end

function editor.draw()
	base.draw()
end

function editor.touch(what, x, y)
	base.touch(what, x, y)
end

function editor.key(what, key)
	base.key(what, key)
end

function editor.message(id, state, sval, nval)
	base.message(id, state, sval, nval)
end

dust.start(editor)