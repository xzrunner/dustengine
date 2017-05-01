local util = require "shaderlab.util"

local M = {}

local NODES = {}

function M.add(name, operanda, operandb, operatre)
	local exists = util.table_find(NODES, name)
	assert(not exists, "already exists")
	local node = {}
	node.operanda = operanda
	node.operandb = operandb
	node.operatre = operatre
	NODES[name] = node

	if name == "gl_FragColor" then
		local mid = {}
		M.parser(mid, name)
	end
end

function M.remove(name)
	util.table_remove_key(NODES, name)
end

function M.print()
	for k, v in pairs(NODES) do
		print(k .. ", a: " .. tostring(v.operanda) .. ", b: " 
			.. tostring(v.operandb) .. ", op: " .. tostring(v.operatre))
	end
end

function M.parser(middle, name)
	if not name then
		print("parser nil name")
		return 
	end

	local node = NODES[name]
	assert(node, "no " .. name)

	if node.operatre == "assign" then
		assert(not node.operandb, "assign with 2 arg")
		local str = name .. " = " .. node.operanda
		print("parser:" .. str)
		middle[#middle+1] = str
		M.parser(node.operanda)
	end
end

return M