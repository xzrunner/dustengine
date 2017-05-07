local c = require "sprite2.c"
local Camera2 = require "dust.camera2"

local gmatch = string.gmatch
local method = c.method
local method_fetch = method.fetch
local method_fetch_by_index = method.fetch_by_index
local method_test = method.point_query
local method_parent = method.get_parent

local get = c.get
local set = c.set

local coord_scene = {x=0, y=0, scale=1}
local coord_ui = {x=0, y=0, scale=1}

local cam_scene = Camera2()
local cam_ui = Camera2()

local method_draw = method.draw
local method_draw_symbol = method.draw_symbol
local method_draw_actor = method.draw_actor

local function _prepare_scene_cam()
    local hw = G.screen_width * G.screen_scale * 0.5
    local hh = G.screen_height * G.screen_scale * 0.5
    local x = coord_scene.x - hw
    local y = -(coord_scene.y - hh)
    local scale = coord_scene.scale
    cam_scene:bind()
    cam_scene:set(-x / scale, -y / scale, 1 / scale)    
end

local function _prepare_ui_cam()
    cam_ui:bind()
    cam_ui:set(0, 0, 1/coord_ui.scale)    
end

function method.draw(spr, srt)
    _prepare_scene_cam()
    return method_draw(spr, srt, cam_scene:get_impl())
end

function method.draw_symbol(spr, srt)
    _prepare_scene_cam()
    return method_draw_symbol(spr, srt, cam_scene:get_impl())
end

function method.draw_actor(spr, srt)
    _prepare_scene_cam()
    return method_draw_actor(spr, srt, cam_scene:get_impl())
end

function method.draw_ui(spr, srt)
    _prepare_ui_cam()
    return method_draw(spr, srt, cam_ui:get_impl())
end

function method.draw_ui_aabb(spr, mat)
    coord_ui.x = 0
    coord_ui.y = 0
    return method.draw_aabb(spr, mat, coord_ui)
end

local sprite_meta = {}

function sprite_meta.__index(spr, key)
    if method[key] then
        return method[key]
    end
    local getter = get[key]
    if getter then
        return getter(spr)
    end

    print("Unsupport get " ..  key)
    return nil

    --local child = fetch(spr, key)

    --if child then
    --    return child
    --else
    --    print("Unsupport get " ..  key)
    --    return nil
    --end
end

function sprite_meta.__newindex(spr, key, v)
    local setter = set[key]
    if setter then
        setter(spr, v)
        return
    end
    assert(debug.getmetatable(v) == sprite_meta, "Need a sprite")
    method.mount(spr, key, v)
end



-- REGISTER_SPR_TABLE = setmetatable({}, {__mode = "kv"})
-- function dump_all_resident_spr()
--     print("=================dump_all_resident_spr==============")
--     for k,v in pairs(REGISTER_SPR_TABLE) do
--         local spr = k
--         print("resident spr:", spr.id)
--     end
--     print("====================================================")
--     print("begin check it!!!!")
--     G.g_vm_snapshot(REGISTER_SPR_TABLE)
-- end

function sprite_meta:__gc()
    c.release_spr(self)
end

local sprite = {}

sprite.get_actor_count = c.get_actor_count
sprite.new_pkg = c.new_pkg
sprite.store_snapshot = c.store_snapshot
sprite.compare_snapshot = c.compare_snapshot

function sprite.new_spr(packname, name)
    local cobj = c.new_spr(packname, name)
    if cobj then
        local ret = debug.setmetatable(cobj, sprite_meta)
        -- REGISTER_SPR_TABLE[ret] = true
        return ret
    end
end

function sprite.new_spr_by_id(id)
    local cobj = c.new_spr_by_id(id)
    if cobj then
        return debug.setmetatable(cobj, sprite_meta)
    end
end

function sprite.new_spr_from_file(filepath)
    local cobj = c.new_spr_from_file(filepath)
    if cobj then
        return debug.setmetatable(cobj, sprite_meta)
    end
end

function sprite.fetch_spr_cached(pkg, spr)
    local cobj = c.fetch_spr_cached(pkg, spr)
    if cobj then
        return debug.setmetatable(cobj, sprite_meta)
    end
end

function sprite.return_spr_cached(spr)
    c.return_spr_cached(spr)
end

function sprite.get_coords()
    return coord_scene, coord_ui
end

function sprite.get_ui_cam()
    return cam_ui
end

function sprite.get_scene_cam()
    return cam_scene
end

local function fetch(spr, path)
    local ret = spr
    for part in gmatch(path, '([^.]+)') do
        ret = method_fetch(ret, part)
        if not ret then
            return nil
        end
    end
    if ret and not getmetatable(ret) then
        return debug.setmetatable(ret, sprite_meta)
    else
        return ret
    end	
end

local function fetch_by_index(spr, index)
    local cobj = method_fetch_by_index(spr, index)
    if cobj and not getmetatable(cobj) then
        return debug.setmetatable(cobj, sprite_meta)
    else
        return cobj
    end 
end

local function test(...)
    local cobj, mat = method_test(...)
    if cobj and not getmetatable(p) then
        return debug.setmetatable(cobj, sprite_meta), mat
    else
        return cobj, mat
    end 
end

local function parent(...)
    local cobj = method_parent(...)
    if cobj and not getmetatable(cobj) then
        return debug.setmetatable(cobj, sprite_meta)
    else
        return cobj
    end
end

method.fetch_raw = fetch
method.fetch = fetch
method.fetch_by_index = fetch_by_index
method.test = test
method.parent = parent

return sprite