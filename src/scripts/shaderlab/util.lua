local M = {}

function M.table_print(table)
	if type(table) ~= "table" then return end
	for k, v in pairs(table) do
		if type(v) == "table" then
			M.table_print(table)
		else
			print(tostring(v))
		end
	end
end

function M.table_remove_value(tab, val)
	for k, v in pairs(tab) do
		if v == val then
			table.remove(tab, k)
			break
		end		
		-- if k == key then
		-- 	tab[key] = nil
		-- 	break
		-- end
	end
end

function M.table_remove_key(tab, key)
	for k, v in pairs(tab) do	
		if k == key then
			tab[key] = nil
			break
		end
	end
end

function M.table_find(tab, val)
	for k, v in pairs(tab) do
		if v == val then
			return true
		end
	end	
	return false
end

return M