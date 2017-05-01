-- simple lighting shader

local dust = require "dust"
local base = require "base"
-- local render = require "ejoy3d.render.c"
require "shaderlab"

local game = {}

function game.load()
	base.load()
	
	-- render.load_mesh("cone", 2, 1)
	-- render.load_mesh("sphere", 1)

	-- N = NormalMatrix * Normal
	local local2view = shaderlab.create("localtoview", 200, 100)
	local normal = shaderlab.create("vertexnormal", 200, 150)
	local multi_normal = shaderlab.create("multiply44", 500, 100)
	local2view:Connect(multi_normal:GetInputNode(1))
	normal:Connect(multi_normal:GetInputNode(2))
--	print(multi_normal:ToCode())

	-- L = normalize(LightPosition)
	local lightposition = shaderlab.create("float3", 150, 250)
	lightposition:SetName("LightPosition")
	local normal_lightpos = shaderlab.create("normalize", 450, 250)
	lightposition:Connect(normal_lightpos:GetInputNode(1))
--	print(normal_lightpos:ToCode())

	if true then return end

	-- E = vec3(0, 0, 1)
	-- H = normalize(L + E)
	local e = shaderlab.create("float3", 200, 350)
	e:SetName("e")
	local add_L_E = shaderlab.create("add", 650, 250)
	normal_lightpos:Connect(add_L_E:GetInputNode(1))
	e:Connect(add_L_E:GetInputNode(2))
	local normal_LE = shaderlab.create("normalize", 750, 400)
	add_L_E:Connect(normal_LE:GetInputNode(1))
--	print(add_L_E:ToCode())
--	print(normal_LE:ToCode())

	-- df = max(0, dot(N, L))
	local dot_NL = shaderlab.create("dotproduct3", 650, 150)
	multi_normal:Connect(dot_NL:GetInputNode(1))
	normal_lightpos:Connect(dot_NL:GetInputNode(2))
	local zero1 = shaderlab.create("float1", 650, 50)
	zero1:SetValue(0)
	local max_NL = shaderlab.create("max", 900, 100)
	zero1:Connect(max_NL:GetInputNode(1))
	dot_NL:Connect(max_NL:GetInputNode(2))
	
	-- print(dot_NL:ToCode())
	-- print(max_NL:ToCode())

	-- sf = max(0, dot(N, H))
	local dot_NH = shaderlab.create("dotproduct3", 900, 350)
	multi_normal:Connect(dot_NH:GetInputNode(1))
	normal_LE:Connect(dot_NH:GetInputNode(2))
	local zero2 = shaderlab.create("float1", 1000, 250)	
	zero2:SetValue(0)
	local max_NH = shaderlab.create("max", 1200, 300)
	zero2:Connect(max_NH:GetInputNode(1))
	dot_NH:Connect(max_NH:GetInputNode(2))

	-- print(dot_NH:ToCode())
	-- print(max_NH:ToCode())

	-- sf = pow(sf, Shininess)
	local shininess = shaderlab.create("float1", 1200, 400)
	shininess:SetName("shininess")
	local pow = shaderlab.create("pow", 1400, 350)
	max_NH:Connect(pow:GetInputNode(1))
	shininess:Connect(pow:GetInputNode(2))

	-- print(pow:ToCode())

	-- df * DiffuseMaterial
	local diffuse = shaderlab.create("float3", 900, 30)
	diffuse:SetName("diffuse")
	local multi_df_diffuse = shaderlab.create("multiply", 1200, 75)
	max_NL:Connect(multi_df_diffuse:GetInputNode(1))
	diffuse:Connect(multi_df_diffuse:GetInputNode(2))

	-- print(multi_df_diffuse:ToCode())

	-- sf * SpecularMaterial
	local specular = shaderlab.create("float3", 1200, 500)
	specular:SetName("specular")
	local multi_sf_specular = shaderlab.create("multiply", 1600, 380)
	pow:Connect(multi_sf_specular:GetInputNode(1))
	specular:Connect(multi_sf_specular:GetInputNode(2))

	-- print(multi_sf_specular:ToCode())

	-- color = ambient + multi_df_diffuse + multi_sf_specular
	local ambient = shaderlab.create("float3", 1100, 170)
	ambient:SetName("ambient")
	local add_ambient_dfdiffuse = shaderlab.create("add", 1400, 100)
	multi_df_diffuse:Connect(add_ambient_dfdiffuse:GetInputNode(1))
	ambient:Connect(add_ambient_dfdiffuse:GetInputNode(2))
	local add_to_color = shaderlab.create("add", 1800, 200)
	add_ambient_dfdiffuse:Connect(add_to_color:GetInputNode(1))
	multi_sf_specular:Connect(add_to_color:GetInputNode(2))

	local outputcolor = shaderlab.create("outputcolor", 2000, 200)
	add_to_color:Connect(outputcolor:GetInputNode(1))

	-- print(add_ambient_dfdiffuse:ToCode())
	-- print(add_to_color:ToCode())
	-- print(outputcolor:ToCode())

	-- gl_Position = Projection * Modelview * Position;
	local localtoview = shaderlab.create("localtoview", 350, 450)
	local xyz = shaderlab.create("vertexxyz", 400, 500)
	local multi_to_pos = shaderlab.create("multiply44", 600, 450)
	localtoview:Connect(multi_to_pos:GetInputNode(1))
	xyz:Connect(multi_to_pos:GetInputNode(2))
	local outputpos = shaderlab.create("outputposition", 800, 450)
	multi_to_pos:Connect(outputpos:GetInputNode(1))

	-- print(multi_to_pos:ToCode())
	-- print(outputpos:ToCode())
end

function game.update()
	base.update()
end

function game.draw()
	base.draw()
end

function game.touch(what, x, y)
	base.touch(what, x, y)
end

function game.key(what, key)
	base.key(what, key)
end

function game.message(id, state, sval, nval)
	base.message(id, state, sval, nval)
end

dust.start(game)


