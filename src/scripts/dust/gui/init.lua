local Sprite = require "dust.Sprite"

gui = {}

-- configurations
gui.config = {}
gui.config["DEFAULTSKIN"] = "Dark"
-- gui.config["DEFAULTSKIN"] = "Light"

gui.input_focus = nil

function gui.register(container, path, name)
	local class = require ("dust.gui." .. path)
	container[name] = class
end

function gui.registerskin(path)
	local skin = require ("dust.gui." .. path)
	local name = skin.name
	local base = skin.base
	if base then
		local newskin = gui.util.DeepCopy(gui.skins[base])
		local skinwidgets = skin.widgets
		for k, v in pairs(skinwidgets) do
			newskin.widgets[k] = v
		end		
		for k, v in pairs(skin) do
			if type(v) == "function" then
				newskin[k] = v
			end
		end		
		skin = newskin
	end
	gui.skins[name] = skin
end

function gui.load()
	require "dust.gui.math"
	require "dust.gui.util"

	gui.widgets = {}
	gui.register(gui.widgets, "widgets.base", "base")
	gui.register(gui.widgets, "widgets.button", "button")
	gui.register(gui.widgets, "widgets.frame", "frame")
	gui.register(gui.widgets, "widgets.checkbox", "checkbox")	
	gui.register(gui.widgets, "widgets.panel", "panel")
	gui.register(gui.widgets, "widgets.collapsiblecategory", "collapsiblecategory")
	gui.register(gui.widgets, "widgets.slider", "slider")
	gui.register(gui.widgets, "widgets.colorchanger", "colorchanger")
	gui.register(gui.widgets, "widgets.text", "text")
	gui.register(gui.widgets, "widgets.colorbutton", "colorbutton")
	gui.register(gui.widgets, "widgets.treelist", "treelist")
	gui.register(gui.widgets, "widgets.input", "input")

	gui.skins = {}
	gui.registerskin("skins.light")
	gui.registerskin("skins.dark")

	gui.roots = {}
end

function gui.create(name, parent)
	local class = gui.widgets[name]
	if class then
		local obj = setmetatable({}, class)
		obj:initialize()
		if parent then
			obj:SetParent(parent)
		else
			table.insert(gui.roots, obj)
		end
		return obj
	end
end

function gui.update()
	for k, v in ipairs(gui.roots) do
		v:Update()
	end
end

function gui.draw()
	for k, v in ipairs(gui.roots) do
		v:Draw()
	end
end

function gui.mousepressed(x, y)
	local x, y = Sprite.get_ui_cam():screen2project(x, y)

	for k, v in ipairs(gui.roots) do
		local skip = v:MousePressed(x, y)
		if skip then return true end
	end
end

function gui.mousereleased(x, y)
	local x, y = Sprite.get_ui_cam():screen2project(x, y)

	for k, v in ipairs(gui.roots) do
		local skip = v:MouseReleased(x, y)
		if skip then return true end
	end
end

function gui.mousemoved(x, y)
	local x, y = Sprite.get_ui_cam():screen2project(x, y)

	for k, v in ipairs(gui.roots) do
		local skip = v:MouseMoved(x, y)
		if skip then return true end
	end
end

function gui.keydown(key)
	-- for k, v in ipairs(gui.roots) do
	-- 	local skip = v:KeyDown(key)
	-- 	if skip then return true end
	-- end

	if gui.input_focus then
		gui.input_focus:KeyDown(key)
	end
end

function gui.keyup(key)
	-- for k, v in ipairs(gui.roots) do
	-- 	local skip = v:KeyUp(key)
	-- 	if skip then return true end
	-- end

	if gui.input_focus then
		gui.input_focus:KeyUp(key)
	end	
end

function gui.pressed(x, y)
	local x, y = Sprite.get_ui_cam():screen2project(x, y)
	
	for k, v in ipairs(gui.roots) do
		if v:GetVisible() then
			local left, right = v:GetPos()
			local width, height = v:GetSize()
			if gui.math.PosInRect(x,y,left,right,width,height) then
				return true
			end			
		end
	end	
	return false
end

gui.load()