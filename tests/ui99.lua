-- all

local dust = require "dust"
local base = require "base"

local game = {}

function game.load()
	base.load()

	require "dust.gui"

	-- button
	local frame = gui.create("frame")
	frame:SetPos(150, 200)
	-- frame:Center()

	local button = gui.create("button", frame)
	button:SetWidth(180):Center()
	button.OnClick = function(object)
		object:SetText("clicked!")
	end

	-- checkbox
	local frame = gui.create("frame")
	frame:SetTitle("Checkbox"):SetHeight(85):SetPos(150, 200)

	local checkbox1 = gui.create("checkbox", frame)
	checkbox1:SetText("Checkbox 1"):SetPos(5, 30)

	local checkbox2 = gui.create("checkbox", frame)
	checkbox2:SetText("Checkbox 2"):SetPos(5, 60)	

	-- panel
	local frame = gui.create("frame")
	frame:SetTitle("Panel"):SetSize(210, 85):SetPos(150, 200)

	local panel = gui.create("panel", frame)
	panel:SetPos(5, 30)	

	-- collapsiblecategory
	local frame = gui.create("frame")
	frame:SetTitle("Collapsible Category"):SetSize(500, 300):SetPos(150, 200)

	local panel = gui.create("panel")
	panel:SetHeight(230)

	local collapsiblecategory = gui.create("collapsiblecategory", frame)
	collapsiblecategory:SetPos(5, 30):SetSize(490, 265):SetText("Category 1")
	collapsiblecategory:SetChild(panel)	

	-- slider
	local frame = gui.create("frame")
	frame:SetTitle("Slider"):SetSize(300, 275):SetPos(150, 200)

	local slider1 = gui.create("slider", frame)
	slider1:SetPos(5, 30):SetWidth(290):SetMinMax(0, 100)
		
	local slider2 = gui.create("slider", frame)
	slider2:SetPos(5, 60):SetHeight(200):SetMinMax(0, 100)
	slider2:SetSlideType("vertical"):CenterX()

	-- colorchanger
	local frame = gui.create("frame")
	frame:SetTitle("Color Changer"):SetSize(500, 255):SetPos(150, 200)

	local colorchanger = gui.create("colorchanger", frame)
	colorchanger:SetPos(0, 0)

	-- treelist
	local frame = gui.create("frame")
	frame:SetTitle("Treelist"):SetSize(300, 400):SetPos(150, 200)

	local treelist = gui.create("treelist", frame)
	treelist:SetPos(0, 0)
	treelist:AddNode("n0"):AddNode("n1"):AddNode("n2"):AddNode("n3")
	treelist:AddNode("n11", "n1"):AddNode("n21", "n2"):AddNode("n22", "n2"):AddNode("n31", "n3")
	treelist:AddNode("n221", "n22")	

	-- text input
	local frame = gui.create("frame")
	frame:SetTitle("Input"):SetSize(200, 100):SetPos(150, 200)

	local input1 = gui.create("input", frame)
	input1:SetPos(5, 30):SetText("string"):SetType("string")

	local input2 = gui.create("input", frame)
	input2:SetPos(5, 60):SetText("number"):SetType("number")
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