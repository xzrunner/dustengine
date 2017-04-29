local c = require "gum.c"

local gum = {}

gum.clear = c.clear
gum.gc = c.gc
gum.get_sym_count = c.get_sym_count
gum.get_img_count = c.get_img_count

gum.create_img = c.create_img
gum.get_img_texid = c.get_img_texid

gum.dtex_set_c2_max_edge = c.dtex_set_c2_max_edge

gum.load_pkg_ids = c.load_pkg_ids
gum.query_pkg_id = c.query_pkg_id

gum.is_async_task_empty = c.is_async_task_empty

function gum.del_pkg(name, pkg_id)
	local id = pkg_id
	if not id then
		id = c.query_pkg_id(name)	
	end
	assert(id >= 0, "error pkg name: " .. name)

	c.del_pkg(id)
end

-- function gum.prepare_pkg(name, pkg_id)
-- 	if c.is_pkg_exists(name) then
-- 		return
-- 	end

-- 	local id = pkg_id
-- 	if not id then
-- 		id = c.query_pkg_id(name)
-- 	end
-- 	assert(id >= 0, "error pkg name: " .. name)

--     local get_res_file = G.g_res_file_svc.get_res_file
--     local epe_path = get_res_file(name..'.epe')
--     if not sysutil.exists(epe_path) then
-- 		return
-- 	end    
--     local ept_path = get_res_file(name..'.ept')
-- 	c.new_pkg(name, id, epe_path, ept_path)

-- 	-- set epe filepath
-- 	local page_count = c.pkg_get_page_count(name)
-- 	for idx = 1, page_count do
-- 		local filepath = get_res_file(name..'.'..idx..'.epe')
-- 		c.pkg_set_page_filepath(name, idx - 1, filepath)
-- 	end

-- 	-- set ept filepath
-- 	local tex_count, lod_count = c.pkg_get_tex_count(id)
-- 	for tex = 1, tex_count do
-- 		for lod = 1, lod_count do
-- 			local filepath = name..'.'..tex
-- 			if lod == 2 then
-- 				filepath = filepath..'.50'
-- 			elseif lod == 3 then
-- 				filepath = filepath..'.25'
-- 			end
-- 			filepath = get_res_file(filepath..'.ept')
-- 			c.pkg_set_tex_filepath(id, tex - 1, lod - 1, filepath)
-- 		end
-- 	end	
-- end

-- function gum.prepare_all_pkgs()
-- 	-- load pkg config
--     local pkg_ids_filepath = G.g_res_file_svc.get_res_file('pkg.bin')
--     c.load_pkg_ids(pkg_ids_filepath)

--     -- set pkg path
-- 	local pkgs = c.get_all_pkg_names()
-- 	for i = 1, #pkgs do
-- 		local pkg = pkgs[i]
-- 		gum.prepare_pkg(pkg)
-- 	end
-- end

gum.set_pkg_release_tag = c.set_pkg_release_tag
gum.release_pkg_after_last_tag = c.release_pkg_after_last_tag

return gum