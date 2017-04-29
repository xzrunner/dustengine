local c = require "dtex2.c"

local dtex2 = {}

dtex2.query_cache = c.query_cache
dtex2.loading_task_empty = c.loading_task_empty

dtex2.cache_pkg_static_query = c.cache_pkg_static_query
dtex2.cache_pkg_static_create = c.cache_pkg_static_create
dtex2.cache_pkg_static_delete = c.cache_pkg_static_delete

dtex2.cache_pkg_static_load = c.cache_pkg_static_load
dtex2.cache_pkg_static_load_finish = c.cache_pkg_static_load_finish

dtex2.cache_pkg_static_is_empty = c.cache_pkg_static_is_empty

return dtex2