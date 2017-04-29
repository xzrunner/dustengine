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
	fw.inject()
end

return dust
