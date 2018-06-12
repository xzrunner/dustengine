#include "RoamApp.h"

#include <terr/TileMapTex.h>
#include <facade/RenderContext.h>
#include <unirender/Shader.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <shaderlab/Buffer.h>
#include <shaderlab/RenderBuffer.h>
#include <painting3/PrimitiveDraw.h>
#include <SM_Calc.h>

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

static const int SIZE = 513;

//const float POS_TRANS_OFF = 1.0f;
const float POS_TRANS_OFF = 0;
const float POS_TRANS_SCALE = 1 / 50.0f;
float pos_world2proj(float world)
{
	return POS_TRANS_OFF + world * POS_TRANS_SCALE;
}

struct Vertex
{
	Vertex(float x, float z, float size)
	{
		position.Set(pos_world2proj(x), 0, pos_world2proj(z));
		texcoords.Set(x / size, z / size);
	}

	sm::vec3 position;
	sm::vec2 texcoords;
};

std::unique_ptr<ur::Shader>       shader = nullptr;
std::unique_ptr<sl::RenderBuffer> vertex_buf = nullptr;

void flush_vertex_buf(ur::DRAW_MODE mode)
{
	vertex_buf->Bind();
	vertex_buf->Update();
	ur::Blackboard::Instance()->GetRenderContext().DrawArrays(mode, 0, vertex_buf->Size());
	vertex_buf->Clear();
}

void send_vertex(float x, float z, float size)
{
	if (vertex_buf->Add(&Vertex(x, z, size), 1)) {
		flush_vertex_buf(ur::DRAW_TRIANGLES);
	}
}

}

namespace terrain
{

RoamApp::RoamApp()
	: rt::Application3D("RoamApp")
	, m_height_map_tex(true)
	, m_tri_pool(DEFAULT_POLYGON_TARGET * 3)
	, m_roam(SIZE, m_tri_pool)
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

	// callback
#ifdef SPLIT_ONLY
	terr::SplitOnlyROAM::CallbackFuncs cb;
#elif defined SPLIT_MERGE
	terr::SplitMergeROAM::CallbackFuncs cb;
#elif defined SPLIT_MERGE_QUEUE
	terr::SplitMergeQueueROAM::CallbackFuncs cb;
#endif
	cb.get_height = [&](int x, int y)->uint8_t
	{
		return m_height_map_tex.GetHeight(x, y);
	};
#ifdef SPLIT_MERGE_QUEUE
	cb.dis_to_camera_square = [&](int x, int y)->float
	{
		sm::vec3 p;
		p.x = pos_world2proj(x);
		p.y = m_height_map_tex.GetHeight(x, y) / 255.0f;
		p.z = pos_world2proj(y);
		return sm::dis_square_pos3_to_pos3(m_camera.GetPos(), p) / POS_TRANS_SCALE;
	};
#else
	cb.dis_to_camera = [&](int x, int y)->float
	{
		sm::vec3 p;
		p.x = pos_world2proj(x);
		p.y = m_height_map_tex.GetHeight(x, y) / 255.0f;
		p.z = pos_world2proj(y);
		return sm::dis_pos3_to_pos3(m_camera.GetPos(), p) / POS_TRANS_SCALE;
	};
#endif // SPLIT_MERGE_QUEUE
	cb.sphere_in_frustum = [&](float x, float y, float radius)->bool
	{
		float px = pos_world2proj(x);
		float py = m_height_map_tex.GetHeight(x, y) / 255.0f;
		float pz = pos_world2proj(y);
		float r = pos_world2proj(radius);
		return m_frustum.SphereInFrustum(px, py, pz, r);
	};
	cb.send_vertex = [&](int x, int y)
	{
		send_vertex(x, y, SIZE);
	};
#if defined SPLIT_MERGE && defined CACHE_VERTEX
	cb.fill_vb = [&](float* vb, float x, float y)
	{
		auto v = Vertex(x, y, SIZE);
		memcpy(vb, &v, sizeof(v));
	};
	cb.draw = [&](float* vb, int vb_n)
	{
		vertex_buf->Add(vb, vb_n);
	};
#endif // SPLIT_MERGE
	m_roam.RegisterCallback(cb);
}

void RoamApp::Init()
{
	//m_height_map_tex.LoadFromRawFile("height128.raw", SIZE);
	//m_height_map_tex.MakeHeightMapPlasma(SIZE, 1.0f);
	m_height_map_tex.MakeHeightMapFault(SIZE, 64, 0, 255, 0.05f);

	m_tile_map_tex.Init();

	m_detail_map_tex = std::make_unique<terr::Texture>("detailMap.tga");

	m_roam.Init();
}

bool RoamApp::Update()
{
	m_frustum.CalculateViewFrustum(m_camera);
	return m_roam.Update();
}

void RoamApp::Draw() const
{
	shader->Use();

	m_height_map_tex.Bind(0);
	m_rc->GetUrRc().BindTexture(m_detail_map_tex->GetTexID(), 1);
	m_tile_map_tex.Bind(2);

	m_roam.Draw();
	flush_vertex_buf(ur::DRAW_TRIANGLES);
}

void RoamApp::UpdateModelView()
{
	shader->Use();
	shader->SetMat4("u_modelview", m_camera.GetModelViewMat().x);
}

void RoamApp::OnKeyDown(rt::KeyType key)
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	switch (key)
	{
	case rt::KEY_W:
		rc.SetPolygonMode(ur::POLYGON_LINE);
		break;
	case rt::KEY_S:
		rc.SetPolygonMode(ur::POLYGON_FILL);
		break;
	}
}

}