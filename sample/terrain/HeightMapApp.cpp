#include "HeightMapApp.h"
#include "TileMapTex.h"

#include <facade/RenderContext.h>
#include <unirender/Shader.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <shaderlab/Buffer.h>
#include <shaderlab/RenderBuffer.h>
#include <painting3/PrimitiveDraw.h>


namespace
{

const char* vs = R"(

attribute vec4 position;
attribute vec2 texcoord;

uniform mat4 u_projection;
uniform mat4 u_modelview;

uniform sampler2D u_tex_heightmap;

varying float v_height;
varying vec2  v_texcoord;

void main()
{
	v_height = texture2D(u_tex_heightmap, texcoord).r;

	vec4 pos = position;
	pos.y = v_height;
	gl_Position = u_projection * u_modelview * pos;

	v_texcoord = texcoord;
}

)";

const char* fs = R"(

uniform sampler2D u_tex_detailmap;

uniform sampler2D u_tex_tilemap0;
uniform sampler2D u_tex_tilemap1;
uniform sampler2D u_tex_tilemap2;
uniform sampler2D u_tex_tilemap3;

varying float v_height;
varying vec2  v_texcoord;

void main()
{
	vec4 tile_col;
	if (v_height < 0.33) {
		float t = v_height / 0.33;
		tile_col = texture2D(u_tex_tilemap0, v_texcoord) * (1 - t) + texture2D(u_tex_tilemap1, v_texcoord) * t;
	} else if (v_height < 0.66) {
		float t = (v_height - 0.33) / 0.33;
		tile_col = texture2D(u_tex_tilemap1, v_texcoord) * (1 - t) + texture2D(u_tex_tilemap2, v_texcoord) * t;
	} else {
		float t = (v_height - 0.66) / 0.33;
		tile_col = texture2D(u_tex_tilemap2, v_texcoord) * (1 - t) + texture2D(u_tex_tilemap3, v_texcoord) * t;
	}

	vec4 detail_col = texture2D(u_tex_detailmap, v_texcoord);

	gl_FragColor = tile_col * detail_col;
}

)";

static const int SIZE = 128;

struct Vertex
{
	Vertex(size_t ix, size_t iz)
	{
		static const float off = -1.0f;
		static const float scale = 1 / 50.0f;

		position.Set(off + ix * scale, 0, off + iz * scale);
		texcoords.Set(ix / static_cast<float>(SIZE), iz / static_cast<float>(SIZE));
	}

	sm::vec3 position;
	sm::vec2 texcoords;
};

std::unique_ptr<ur::Shader>       shader = nullptr;
std::unique_ptr<sl::RenderBuffer> vertex_buf = nullptr;
}

namespace terrain
{

HeightMapApp::HeightMapApp()
	: rt::Application3D("HeightMapApp")
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();

	std::vector<std::string> textures;
	textures.push_back("u_tex_heightmap");
	textures.push_back("u_tex_detailmap");
	textures.push_back("u_tex_tilemap0");
	textures.push_back("u_tex_tilemap1");
	textures.push_back("u_tex_tilemap2");
	textures.push_back("u_tex_tilemap3");

	CU_VEC<ur::VertexAttrib> layout;
	layout.push_back(ur::VertexAttrib("position", 3, 4));
	layout.push_back(ur::VertexAttrib("texcoord", 2, 4));

	shader = std::make_unique<ur::Shader>(&rc, vs, fs, textures, layout);

	size_t vertex_sz = sizeof(float) * (3 + 2);
	size_t max_vertex = SIZE * SIZE * 2;
	sl::Buffer* buf = new sl::Buffer(vertex_sz, max_vertex);
	vertex_buf = std::make_unique<sl::RenderBuffer>(rc, ur::VERTEXBUFFER, vertex_sz, max_vertex, buf);

	// update mat
	shader->Use();
	shader->SetMat4("u_projection", m_camera.GetProjectionMat().x);
	shader->SetMat4("u_modelview", m_camera.GetModelViewMat().x);
}

void HeightMapApp::Init()
{
	//m_height_map_tex.LoadFromRawFile("height128.raw", SIZE);
	//m_height_map_tex.MakeHeightMapPlasma(SIZE, 1.0f);
	m_height_map_tex.MakeHeightMapFault(SIZE, 64, 0, 255, 0.05f);

	m_tile_map_tex.Init();

	m_detail_map_tex = std::make_unique<Texture>("detailMap.tga");
}

void HeightMapApp::Draw() const
{
	shader->Use();

	m_height_map_tex.Bind(0);
	m_rc->GetUrRc().BindTexture(m_detail_map_tex->GetTexID(), 1);
	m_tile_map_tex.Bind(2);

	bool new_line = false;
	for (size_t z = 0; z < SIZE - 1; ++z)
	{
		for (size_t x = 0; x < SIZE; ++x)
		{
			Vertex v0(x, z);
			if (new_line)
			{
				vertex_buf->Add(&v0, 1);
				new_line = false;
			}
			vertex_buf->Add(&v0, 1);

			Vertex v1(x, z + 1);
			vertex_buf->Add(&v1, 1);
			if (x == SIZE - 1)
			{
				vertex_buf->Add(&v1, 1);
				new_line = true;
			}
		}
	}

	vertex_buf->Bind();
	vertex_buf->Update();
	ur::Blackboard::Instance()->GetRenderContext().DrawArrays(
		ur::DRAW_TRIANGLE_STRIP, 0, vertex_buf->Size());
	vertex_buf->Clear();
}

void HeightMapApp::UpdateModelView()
{
	shader->Use();
	shader->SetMat4("u_modelview", m_camera.GetModelViewMat().x);
}

}