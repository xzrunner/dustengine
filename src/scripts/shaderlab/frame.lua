require "dust.gui"

local M = {}

local frame = gui.create("frame")
frame:SetTitle("Nodes"):SetSize(160, 600):SetPos(0, 0)
M.frame = frame

local NODES = {
	"Camera", "Transform", "Vertex", "Texture", "Value", "Extra", "Math",
	"Manipulation", "Lights", "Node Group",
	["Camera"] = {
		"Camera Direction", "Camera Position",
		["Camera Direction"] = "",
		["Camera Position"] = "",
	},
	["Transform"] = {
		"Matrix", "Operation",
		["Matrix"] = {
			"Local To World Matrix", "Local To Camera Matrix", "Local To View Matrix",
			"Local To Screen Matrix", "Camera To View Matrix",
			["Local To World Matrix"] = "",
			["Local To Camera Matrix"] = "",
			["Local To View Matrix"] = "localtoview",
			["Local To Screen Matrix"] = "",
			["Camera To View Matrix"] = "",
		},
		["Operation"] = {},
	},
	["Vertex"] = {
		"Vertex XYZ", "Vertex Normal", "Vertex UV", "Vertex Clipspace Position",
		["Vertex XYZ"] = "vertexxyz",
		["Vertex Normal"] = "vertexnormal",
		["Vertex UV"] = "",
		["Vertex Clipspace Position"] = "vertexclipspaceposition",
	},
	["Texture"] = {},
	["Value"] = {
		"Float 1", "Float 2", "Float 3", "Float 4", "RGB Color", "RGBA Color",
		["Float 1"] = "float1",
		["Float 2"] = "float2",
		["Float 3"] = "float3",
		["Float 4"] = "float4",
		["RGB Color"] = "",
		["RGBA Color"] = "rgbacolor",
	},
	["Extra"] = {
		["Interpolate"] = "",
		["Sound"] = "",
		["Mouse X"] = "",
		["Mouse Y"] = "",
	},
	["Math"] = {
		"Add", "Substract", "Multi", "Divide", "Multiply 4x4", "Length",
		"Normalize", "Cross Product", "Dot Product 2", "Dot Product 3", "Dot Product 4",
		"Min", "Max", "Pow",
		["Add"] = "add",
		["Substract"] = "aubstract",
		["Multi"] = "multiply",
		["Divide"] = "divide",
		["Multiply 4x4"] = "multiply44",
		["Length"] = "",
		["Normalize"] = "normalize",
		["Cross Product"] = "",
		["Dot Product 2"] = "",
		["Dot Product 3"] = "dotproduct3",
		["Dot Product 4"] = "",
		["Min"] = "min",
		["Max"] = "max",
		["Pow"] = "pow",
	},
	["Manipulation"] = {},
	["Lights"] = {},
	["Node Group"] = {},
}

local NAME_TO_NODE = {}
local treelist = gui.create("treelist", frame)
local function _buildtreelist(nodes, parent)
	for k, v in ipairs(nodes) do
		local node = nodes[v]
		treelist:AddNode(v, parent)
		if type(node) == "table" then
			_buildtreelist(node, v)
		elseif type(node) == "string" then
			NAME_TO_NODE[v] = node
		end
	end
end
treelist:SetSize(150, 565):SetPos(0, 0)
_buildtreelist(NODES, nil)
M.treelist = treelist


function M.draw()
	frame:Draw()
end

function M.mousepressed(x, y)
	M.selected = M.treelist:GetSelectedName()
end

function M.mousereleased(x, y)
	if M.selected then
		local node = NAME_TO_NODE[M.selected]
		shaderlab.create(node, x, y)
	end
	M.selected = nil
end

function M.rightmousepressed(x, y)
	M.rightpressed = true
	M.xlast = x
	M.ylast = y	
end

function M.rightmousereleased(x, y)
	M.rightpressed = false
end

function M.mousemoved(x, y)
	if M.rightpressed then
		local dx = x - M.xlast
		local dy = y - M.ylast;
		local _x, _y = M.frame:GetPos()
		_x = _x - dx
		_y = _y - dy
		M.frame:SetPos(_x, _y)
		M.xlast = x
		M.ylast = y
	end
end

return M