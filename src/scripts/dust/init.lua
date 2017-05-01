local fw = require "dust.framework"

local dust = {}

local touch = {
	"BEGIN",
	"END",
	"MOVE",
	"CANCEL"
}

local gesture = {
	"PAN",
	"TAP",
	"PINCH",
    "PRESS",
    "DOUBLE_TAP",
}

function dust.start(callback)
	fw.DUST_LOAD = assert(callback.load)
	fw.DUST_UPDATE = assert(callback.update)
	fw.DUST_DRAW = assert(callback.draw)

	fw.DUST_TOUCH = function(x,y,what,id)
		return callback.touch(touch[what],x,y,id)
	end
	fw.DUST_KEY = assert(callback.key)
	fw.DUST_MESSAGE = assert(callback.message)

	fw.inject()
end

return dust
