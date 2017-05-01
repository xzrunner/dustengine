-- treelist

local dust = require "dust"
local base = require "base"

local game = {}

function game.load()
	base.load()

	require "dust.gui"

	local frame = gui.create("frame")
	frame:SetTitle("Treelist"):SetSize(300, 400):SetPos(150, 200)

	local treelist = gui.create("treelist", frame)
	treelist:SetPos(0, 0)
	treelist:AddNode("n0"):AddNode("n1"):AddNode("n2"):AddNode("n3")
	treelist:AddNode("n11", "n1"):AddNode("n21", "n2"):AddNode("n22", "n2"):AddNode("n31", "n3")
	treelist:AddNode("n221", "n22")	
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