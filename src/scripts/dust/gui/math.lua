gui.math = {}

function gui.math.PosInRect(px, py, rx, ry, rw, rh)
	return px > rx and px < rx + rw 
	   and py > ry and py < ry + rh
end

function gui.math.PosInCircle(px, py, cx, cy, radius)
	local dx = px - cx
	local dy = py - cy
	return dx*dx+dy*dy < radius*radius
end

function gui.math.Round(num, idp)
	local mult = 10^(idp or 0)
    if num >= 0 then 
		return math.floor(num * mult + 0.5) / mult
    else 
		return math.ceil(num * mult - 0.5) / mult 
	end
end