#include "HeightMap.h"

#include <facade/RenderContext.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <unirender/Shader.h>
#include <shaderlab/Buffer.h>
#include <shaderlab/RenderBuffer.h>
#include <painting3/PrimitiveDraw.h>

#include <fstream>

namespace
{

const char* vs = R"(

attribute vec4 position;
attribute vec2 texcoord;

uniform mat4 u_projection;
uniform mat4 u_modelview;

uniform sampler2D u_tex_heightmap;

varying float v_height;

void main()
{
	v_height = texture2D(u_tex_heightmap, texcoord).r;

	vec4 pos = position;
	pos.y = v_height;
	gl_Position = u_projection * u_modelview * pos;
}

)";

const char* fs = R"(

varying float v_height;

void main()
{
	gl_FragColor = vec4(v_height, v_height, v_height, 1);
}

)";

struct Vertex
{
	sm::vec3 position;
	sm::vec2 texcoords;
};

std::unique_ptr<ur::Shader>       shader = nullptr;
std::unique_ptr<sl::RenderBuffer> vertex_buf = nullptr;

unsigned int height_map_texid = 0;

static const size_t SIZE = 128;

}

namespace terrain
{

HeightMap::HeightMap()
	: rt::Application3D("HeightMap")
{


	auto& rc = ur::Blackboard::Instance()->GetRenderContext();

	CU_VEC<ur::VertexAttrib> layout;
	layout.push_back(ur::VertexAttrib("position", 3, 4));
	layout.push_back(ur::VertexAttrib("texcoord", 2, 4));

	shader = std::make_unique<ur::Shader>(&rc, vs, fs, layout);

	size_t vertex_sz = sizeof(float) * (3 + 2);
	size_t max_vertex = SIZE * SIZE * 2;
	sl::Buffer* buf = new sl::Buffer(vertex_sz, max_vertex);
	vertex_buf = std::make_unique<sl::RenderBuffer>(rc, ur::VERTEXBUFFER, vertex_sz, max_vertex, buf);

	// update mat
	shader->Use();
	shader->SetMat4("u_projection", m_camera.GetProjectionMat().x);
	shader->SetMat4("u_modelview", m_camera.GetModelViewMat().x);
}

void HeightMap::Init()
{
	height_map_texid = LoadHeightMapTex("height128.raw", SIZE);
}

void HeightMap::Draw() const
{
	shader->Use();

	m_rc->GetUrRc().BindTexture(height_map_texid, 0);

	for (size_t z = 0; z < SIZE - 1; ++z)
	{
		for (size_t x = 0; x < SIZE - 1; ++x)
		{
			Vertex v;

			static const float off = -1.0f;
			static const float scale = 1 / 50.0f;

			v.position.Set(off + x * scale, 0, off + z * scale);
			v.texcoords.Set(x / static_cast<float>(SIZE), z / static_cast<float>(SIZE));
			vertex_buf->Add(&v, 1);

			v.position.Set(off + x * scale, 0, off + (z + 1) * scale);
			v.texcoords.Set(x / static_cast<float>(SIZE), (z + 1) / static_cast<float>(SIZE));
			vertex_buf->Add(&v, 1);
		}
	}

	vertex_buf->Bind();
	vertex_buf->Update();
	ur::Blackboard::Instance()->GetRenderContext().DrawArrays(
		ur::DRAW_TRIANGLE_STRIP, 0, vertex_buf->Size());
	vertex_buf->Clear();
}

void HeightMap::UpdateModelView()
{
	shader->Use();
	shader->SetMat4("u_modelview", m_camera.GetModelViewMat().x);
}

unsigned int HeightMap::LoadHeightMapTex(const char* filepath, size_t size) const
{
	std::ifstream fin(filepath, std::ios::binary | std::ios::ate);
	if (fin.fail()) {
		return 0;
	}

	size_t sz = static_cast<size_t>(fin.tellg());
	fin.seekg(0, std::ios::beg);

	std::vector<char> buffer(size * size);
	if (fin.read(buffer.data(), size * size))
	{
		auto& rc = ur::Blackboard::Instance()->GetRenderContext();
		return rc.CreateTexture(buffer.data(), size, size, ur::TEXTURE_A8);
	}
	else
	{
		return 0;
	}
}

}