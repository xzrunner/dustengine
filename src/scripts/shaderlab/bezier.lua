-- local Graphics = require "dust.graphics"

-- local M = {}
-- M.__index = M

-- function M.Init(front, back)
-- 	self.front = front
-- 	self.back = back

-- 	local mid = (front + back) * 0.5
-- 	local off = (back - front) * 0.5
-- 	self.ctrl1 = mid + off.perpendicular()
-- 	self.ctrl2 = mid - off.perpendicular()
-- end

-- function M.Draw()
-- 	Graphics.line({self.front.x, self.front.y, self.back.x, self.back.y})
-- end

-- return M