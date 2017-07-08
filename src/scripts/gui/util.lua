gui.util = {}

function gui.util.DeepCopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in next, orig, nil do
            copy[gui.util.DeepCopy(orig_key)] = gui.util.DeepCopy(orig_value)
        end
        setmetatable(copy, gui.util.DeepCopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end