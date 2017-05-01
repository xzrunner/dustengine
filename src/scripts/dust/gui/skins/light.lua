local Gtxt = require "dust.gtxt"
local Graphics = require "dust.graphics"
local style = require "dust.gui.style"

local skin = {}

skin.name = "Light"

local smallfont = 14
local middlefont = 16

-- widgets
skin.widgets = {}

-- common
skin.widgets.bordercolor                           = { 96,  96,  96, 255}

-- text
skin.widgets.text_color                            = {  0,   0,   0, 255}

-- frame
skin.widgets.frame_head_height                     = 25
skin.widgets.frame_head_color                      = {  0, 196, 255, 255}
skin.widgets.frame_head_border_color               = {255, 255, 255,  70}
skin.widgets.frame_body_color                      = {232, 232, 232, 255}
skin.widgets.frame_body_border_color               = {220, 220, 220, 255}
-- skin.widgets.frame_body_color                   = {22, 22, 22, 255}
skin.widgets.frame_title_color                     = {255, 255, 255, 255}
skin.widgets.frame_title_font                      = middlefont

-- button
skin.widgets.button_body_released_color            = {232, 232, 232, 255}
skin.widgets.button_body_pressed_color             = {  0, 196, 232, 255}
skin.widgets.button_text_released_color            = {  0,   0,   0, 255}
skin.widgets.button_text_pressed_color             = {255, 255, 255, 255}
skin.widgets.button_text_font                      = smallfont

-- checkbox
skin.widgets.checkbox_body_color                   = {255, 255, 255, 255}
skin.widgets.checkbox_check_color                  = {128, 204, 255, 255}
skin.widgets.checkbox_text_color                   = {  0,   0,   0, 255}
skin.widgets.checkbox_text_font                    = smallfont

-- panel
skin.widgets.panel_body_color                      = {255, 255, 255, 200}
skin.widgets.panel_border_color                    = { 78, 102, 104, 255}

-- collapsiblecategory
skin.widgets.collapsiblecategory_text_color        = {255, 255, 255, 255}

-- slider
skin.widgets.slider_bar_outline_color              = {220, 220, 220, 255}
skin.widgets.slider_button_color                   = {255, 255, 255, 255}

-- treelist
skin.widgets.treelist_bg_color                     = {  0, 196, 255,  32}
skin.widgets.treelist_selected_color               = {  0, 196, 255, 128}
skin.widgets.treelist_font                         = smallfont
skin.widgets.treelist_text_color                   = { 32,  32,  32, 255}

-- input
skin.widgets.input_bg_color                        = {  0, 196, 255, 255}
skin.widgets.input_flag_color                      = { 96,  96,  96, 255}

function skin.DrawOutline(x, y, width, height)
	Graphics.draw_rect(true, x, y, width, 1)
	Graphics.draw_rect(true, x, y + height - 1, width, 1)
	Graphics.draw_rect(true, x, y, 1, height)
	Graphics.draw_rect(true, x + width - 1, y, 1, height)
end

function skin.DrawFrame(obj)
	local skin = obj:GetSkin()
	local widgets = skin.widgets
	local x, y = obj:GetPos()
	local w, h = obj:GetSize()
	local title = obj:GetTitle()

	Graphics.set_color(widgets.frame_head_color)
	Graphics.draw_rect(true, x, y, w, widgets.frame_head_height)

	Graphics.set_color(widgets.frame_body_color)
	Graphics.draw_rect(true, x, y+widgets.frame_head_height, w, h-widgets.frame_head_height)

	Gtxt.print(title, x+4, y+4, widgets.frame_title_font, widgets.frame_title_color)

	Graphics.set_color(widgets.bordercolor)
	skin.DrawOutline(x, y, w, h)

	Graphics.set_color(widgets.frame_head_border_color)
	skin.DrawOutline(x + 1, y + 1, w - 2, 24)
	
	Graphics.set_color(widgets.frame_body_border_color)
	skin.DrawOutline(x + 1, y + 25, w - 2, h - 26)
end

function skin.DrawButton(obj)
	local skin = obj:GetSkin()
	local widgets = skin.widgets
	local x, y = obj:GetPos()
	local w, h = obj:GetSize()
	local text = obj:GetText()
	local state = obj:GetState()

	local body_color, text_color
	if state == "released" then
		body_color = widgets.button_body_released_color
		text_color = widgets.button_text_released_color
	else
		body_color = widgets.button_body_pressed_color
		text_color = widgets.button_text_pressed_color		
	end
	Graphics.set_color(body_color)
	Graphics.draw_rect(true, x, y, w, h)
	Gtxt.print(text, x + 4, y + 4, widgets.button_text_font, text_color)

	Graphics.set_color(widgets.bordercolor)
	skin.DrawOutline(x, y, w, h)	
end

function skin.DrawCheckBox(obj)
	local skin = obj:GetSkin()
	local widgets = skin.widgets
	local x, y = obj:GetPos()
	local w, h = obj:GetSize()

	local bodycolor = widgets.checkbox_body_color
	Graphics.set_color(bodycolor)
	Graphics.draw_rect(true, x, y, w, h)
	
	Graphics.set_color(widgets.bordercolor)
	skin.DrawOutline(x, y, w, h)
	
	if obj:GetChecked() then
		local checkcolor = widgets.checkbox_check_color
		Graphics.set_color(checkcolor)
		Graphics.draw_rect(true, x + 4, y + 4, w - 8, h - 8)
	end

	local text = obj:GetText()
	local textcolor = widgets.checkbox_text_color
	local textfont = widgets.checkbox_text_font
	Gtxt.print(text, x + 4 + w, y + 4, textfont, textcolor)
end

function skin.DrawPanel(obj)
	local skin = obj:GetSkin()
	local widgets = skin.widgets
	local x, y = obj:GetPos()
	local w, h = obj:GetSize()
	local bodycolor = widgets.panel_body_color

	Graphics.set_color(bodycolor)
	Graphics.draw_rect(true, x, y, w, h)
	
	Graphics.set_color(widgets.panel_border_color)
	skin.DrawOutline(x + 1, y + 1, w - 2, h - 2)
	
	Graphics.set_color(widgets.bordercolor)
	skin.DrawOutline(x, y, w, h)
end

function skin.DrawCollapsibleCategory(obj)
	local skin = obj:GetSkin()
	local widgets = skin.widgets
	local x, y = obj:GetPos()
	local w, h = obj:GetSize()
	local head_height = obj:GetClosedHeight()

	Graphics.set_color(widgets.frame_head_color)
	Graphics.draw_rect(true, x+1, y+1, w-2, head_height)

	Gtxt.print(obj:GetTitle(), x+4, y+4, widgets.frame_title_font, widgets.frame_body_color)

	Graphics.set_color(widgets.bordercolor)
	local sw = head_height-5
	local sh = 4
	local sx = x+w-head_height-10
	local sy = y+(head_height-sh)/2
	Graphics.draw_rect(true,sx,sy,sw,sh)	
	if obj:IsOpen() then
		Graphics.set_color(widgets.frame_body_color)
		Graphics.draw_rect(true, x, y+head_height, w, h-head_height)	

		Graphics.set_color(widgets.bordercolor)
		skin.DrawOutline(x, y, w, h)
	else
		sx = sx+sw/2-sh/2
		sy = y+6/2
		Graphics.draw_rect(true,sx,sy,sh,sw)

		Graphics.set_color(255, 255, 255, 70)
		skin.DrawOutline(x + 1, y + 1, w - 2, head_height)
	end
end

function skin.DrawSlider(obj)
	local skin = obj:GetSkin()
	local widgets = skin.widgets
	local x, y = obj:GetPos()
	local w, h = obj:GetSize()
	local slidtype = obj:GetSlideType()
	local baroutlinecolor = widgets.slider_bar_outline_color
	local val = obj:GetValue()
	local min, max = obj:GetMinMax()
	local buttonwidth, buttonheight = obj:GetButtonSize()

	local cx = x
	local cy = y
	local bw, bh
	if slidtype == "horizontal" then
		Graphics.set_color(baroutlinecolor)
		Graphics.draw_rect(true, x, y + h/2 - 5, w, 10)
		Graphics.set_color(widgets.bordercolor)
		Graphics.draw_rect(true, x + 5, y + h/2, w - 10, 1)

		cx = x + w * (val / (max - min))
		cy = y + h / 2
		bw = buttonwidth
		bh = buttonheight
	elseif slidtype == "vertical" then
		Graphics.set_color(baroutlinecolor)
		Graphics.draw_rect(true, x + w/2 - 5, y, 10, h)
		Graphics.set_color(widgets.bordercolor)
		Graphics.draw_rect(true, x + w/2, y + 5, 1, h - 10)

		cy = y + h * (val / (max - min))
		cx = x + w / 2
		bw = buttonheight
		bh = buttonwidth
	end

	cx = cx - bw / 2 + 1
	cy = cy - bh / 2 + 1
	Graphics.set_color(widgets.slider_button_color)
	Graphics.draw_rect(true, cx, cy, bw, bh)
	Graphics.set_color(widgets.bordercolor)
	skin.DrawOutline(cx, cy, bw, bh)
end

function skin.DrawText(obj)
	local skin = obj:GetSkin()
	local widgets = skin.widgets

	local x, y = obj:GetPos()	
	local font = obj:GetFont()
	local text = obj:GetText()
	Gtxt.print(text, x, y, font, widgets.text_color)
end

function skin.DrawColorbutton(obj)
	local skin = obj:GetSkin()
	local widgets = skin.widgets

	local x, y = obj:GetPos()
	local w, h = obj:GetSize()
	local color = obj:GetColor()
	Graphics.set_color(color)
	Graphics.draw_rect(true, x, y, w, h)
end

local function _drawtreelist(x, y, nodes, config)
	x = x + config.indent
	for k, v in ipairs(nodes) do
		Gtxt.print(v.name, x, y, config.font, config.font_color)
		y = y + config.height
		if v.nodes then
			local ori_x = x - config.indent
			local ori_y = y - config.height
			Graphics.draw_rect(true, ori_x+6, ori_y+7, 6, 2)
			if v.expand then
				x, y = _drawtreelist(x, y, v.nodes, config)
			else
				Graphics.draw_rect(true, ori_x+8, ori_y+5, 2, 6)
			end
		end
	end	
	x = x - config.indent
	return x, y
end

function skin.DrawTreelist(obj)
	local skin = obj:GetSkin()
	local widgets = skin.widgets

	local x, y = obj:GetPos()
	x = x + style.treelist_left_offset
	y = y + style.treelist_top_offset
	local w, h = obj:GetSize()
	local bgcolor = widgets.treelist_bg_color
	local bordercolor = widgets.bordercolor
	Graphics.set_color(bgcolor)
	Graphics.draw_rect(true, x, y, w, h)
	Graphics.set_color(bordercolor)
	Graphics.draw_rect(false, x, y, w, h)	

	local config = {}
	config.indent = style.treelist_indent
	config.height = style.treelist_node_height	
	config.font = widgets.treelist_font
	config.font_color = widgets.treelist_text_color

	local selected = obj:GetSelectedPos()
	if selected ~= -1 then
		local h = style.treelist_node_height
		Graphics.set_color(widgets.treelist_selected_color)
		Graphics.draw_rect(true, x, y+h*selected, w-1, h)
	end

	local nodes = obj:GetNodes()
	_drawtreelist(x, y, nodes, config)
end

function skin.DrawInput(obj)
	local skin = obj:GetSkin()
	local widgets = skin.widgets

	local x, y = obj:GetPos()
	local w, h = obj:GetSize()
	Graphics.set_color(widgets.input_bg_color)
	Graphics.draw_rect(true, x, y, w, h)

	local font = obj:GetFont()
	local text = obj:GetText()
	Gtxt.print(text, x, y, font, widgets.text_color)

	if gui.input_focus == obj then
		local x = x + 8*#obj:GetText()
		Graphics.set_color(widgets.input_flag_color)
		Graphics.draw_rect(true, x, y, 2, h)
	end
end

return skin