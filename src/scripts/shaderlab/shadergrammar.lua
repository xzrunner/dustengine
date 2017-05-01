local M = {}
M.__index = M

function M:initialize()
	self:Clear()
end

function M:Clear()
	self.attributes = {}
	self.uniforms = {}

	self.middles = {}	
end

function M:Parser()
	local ret = ""
	-- attribute
	for i = 1, #self.attributes do
		local attribute = self.attributes[i]
		local str = string.format("attribute %s %s;", attribute.type, attribute.name)
		ret = ret .. str .. '\n'
	end
	ret = ret .. '\n'
	-- uniform
	for i = 1, #self.uniforms do
		local uniform = self.uniforms[i]
		local str = string.format("uniform %s %s;", uniform.type, uniform.name)
		ret = ret .. str .. '\n'
	end
	ret = ret .. '\n'
	-- varying
	-- main	
	ret = ret .. 'void main(void)\n'
	ret = ret .. '{\n'
	for i = 1, #self.middles do
		ret = ret .. self.middles[i] .. '\n'
	end
	ret = ret .. '}\n'
	return ret	
end

return M