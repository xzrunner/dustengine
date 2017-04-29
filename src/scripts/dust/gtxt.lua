local c = require "gtxt.c"

local gtxt = {}

function gtxt.create()
    --c.create(bitmap, glyph)
    --bitmap是指画的时候的pixels，可以重复，同样的字用一个
    --glyph是指排版的layout，不重复，每个字一个
    c.init(512, 1024)
end

function gtxt.add_user_font(name, user_font)
    for _, v in ipairs(user_font) do
        c.add_user_font(v.char, v.pkg, v.name)
    end
end

gtxt.add_font = c.add_font
gtxt.add_color = c.add_color

return gtxt