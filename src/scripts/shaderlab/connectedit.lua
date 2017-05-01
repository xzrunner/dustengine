local Graphics = require "dust.graphics"
local Sprite = require "dust.Sprite"

local M = {}

local LINE_COLOR    = { 83, 255, 255, 255}
local LINE_WIDTH    = 3

function M.mousepressed(x, y)
	local x, y = Sprite.get_ui_cam():screen2project(x, y)
	for k, v in ipairs(shaderlab.nodes) do
		local interface = v:TouchInput(x, y)
		if interface then
			assert(#interface.connected == 1, "interface.connected ~= 1")
			local node = interface.connected[1]:GetNode()
			node:Deconnect(interface)
			M.pressed = node
			return true
		end
		if v:TouchOutput(x, y) then
			M.pressed = v
			return true
		end
	end
	return false	
end

function M.mousereleased(x, y)
	if M.pressed then
		local x, y = Sprite.get_ui_cam():screen2project(x, y)
		for k, v in ipairs(shaderlab.nodes) do
			local interface = v:TouchInput(x, y)
			if interface then
				M.pressed:Connect(interface)
			end
		end				
	end

	M.pressed = nil
	M.xcurr = nil
	M.ycurr = nil	
end

function M.mousemoved(x, y)
	local x, y = Sprite.get_ui_cam():screen2project(x, y)
	M.xcurr = x
	M.ycurr = y	
end

function M.draw()
	print("connect draw 0")
	if M.pressed and M.xcurr and M.ycurr then
	print("connect draw 1")		
		local x1, y1 = M.pressed:GetOutputPos()
		local x2 = M.xcurr
		local y2 = M.ycurr
		Graphics.set_color(LINE_COLOR)
		Graphics.set_linewidth(LINE_WIDTH)
		Graphics.line(x1, y1, x2, y2)
	end	
end

return M