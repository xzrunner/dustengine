
local pairs = pairs
local type = type
local format = string.format
local tostring = tostring
local concat = table.concat

-- 不处理循环嵌套等情况, 保持这个函数简单高效
-- 如有需要, 可以再实现一个完善版本
local function _table2str(lua_table, indent, result_table, n)
    local indent_str = "  "
    indent = indent or 0

    for k, v in pairs(lua_table) do
        local tpk, tpv = type(k), type(v)
        if tpk == "string" then
            k = format("%q", k)
        else
            k = tostring(k)
        end
        if tpv == "table" then
            for i=1,indent do
                n=n+1; result_table[n] = indent_str
            end
            n=n+1; result_table[n] = "["
            n=n+1; result_table[n] = k
            n=n+1; result_table[n] = "]={\n"

            -- recursive
            n = _table2str(v, indent+1, result_table, n)

            for i=1,indent do
                n=n+1; result_table[n] = indent_str
            end
            n=n+1; result_table[n] = "},\n"
        else
            if tpv == "string" then
                v = format("%q", v)
            else
                v = tostring(v)
            end
            for i=1,indent do
                n=n+1; result_table[n] = indent_str
            end
            n=n+1; result_table[n] = "["
            n=n+1; result_table[n] = k
            n=n+1; result_table[n] = "]="
            n=n+1; result_table[n] = v
            n=n+1; result_table[n] = ",\n"
        end
    end

    return n
end

return function(lua_table)
    local _seri_table = {}
    local n = 0  -- length of _seri_table
    n=n+1; _seri_table[n] = '{\n'
    n = _table2str(lua_table, 1, _seri_table, n)
    n=n+1; _seri_table[n] = '}'

    return concat(_seri_table, '')
end

