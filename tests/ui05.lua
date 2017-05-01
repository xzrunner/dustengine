-- collapsiblecategory

local dust = require "dust"
local base = require "base"

local game = {}

function game.load()
	base.load()
	
	require "dust.gui"

	local frame = gui.create("frame")
	frame:SetTitle("Collapsible Category"):SetSize(500, 300):SetPos(150, 200)

	local panel = gui.create("panel")
	panel:SetHeight(230)

	local collapsiblecategory = gui.create("collapsiblecategory", frame)
	collapsiblecategory:SetPos(5, 30):SetSize(490, 265):SetText("Category 1")
	collapsiblecategory:SetChild(panel)
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