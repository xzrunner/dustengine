#include "SimpleSceneApp.h"

#include <terr/TileMapTex.h>
#include <terr/TextureGen.h>
#include <terr/TextureLoader.h>
#include <facade/RenderContext.h>
#include <unirender/Shader.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <shaderlab/Buffer.h>
#include <shaderlab/RenderBuffer.h>
#include <painting3/PrimitiveDraw.h>
#include <SM_Calc.h>

#include <fstream>

#ifndef M_PI
#define M_PI		3.14159265358979323846f
#endif

namespace
{

const char* vs = R"(

attribute vec4 position;
attribute vec2 texcoord;

uniform mat4 u_projection;
uniform mat4 u_modelview;

uniform vec3 u_cam_pos;

uniform sampler2D u_tex_heightmap;

varying vec2 v_texcoord;
varying float v_alpha;

void main()
{
	vec4 pos = position;
	float height = texture2D(u_tex_heightmap, texcoord).r;
	pos.y = height * 20;
	gl_Position = u_projection * u_modelview * pos;

	float dis = length(u_cam_pos - position.xyz);
	v_alpha = clamp(dis / 100.0, 0.5, 1);

	v_texcoord = texcoord;
}

)";

const char* fs = R"(

uniform sampler2D u_tex_detailmap;

uniform sampler2D u_tex_blendmap;
uniform sampler2D u_tex_splat0;
uniform sampler2D u_tex_splat1;
uniform sampler2D u_tex_splat2;

varying vec2 v_texcoord;
varying float v_alpha;

void main()
{
	vec2 texcoord_fix = vec2(v_texcoord.x, 1 - v_texcoord.y);
	vec4 blend = texture2D(u_tex_blendmap, v_texcoord);
	vec4 tile_col =
		texture2D(u_tex_splat0, v_texcoord) * blend.y +
		texture2D(u_tex_splat1, v_texcoord) * blend.z +
		texture2D(u_tex_splat2, v_texcoord) * blend.w;
	vec4 detail_col = texture2D(u_tex_detailmap, texcoord_fix);

	gl_FragColor = vec4(detail_col.rgb * v_alpha + tile_col.rgb * (1 - v_alpha), 1);

//	gl_FragColor = vec4(v_alpha, 1 - v_alpha, 0, 1);
}

)";

static const int SCENE_SIZE = 512;

const float SCALE[] = { 50, 50, 10 };
const float LIGHT_DIR[] = { 1, 0, -1 };

const float POS_TRANS_OFF = 0;
const float POS_TRANS_SCALE = 1 / 5.0f;
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

SimpleSceneApp::SimpleSceneApp()
	: rt::Application3D("SimpleSceneApp")
	, m_height_map_tex(true)
	, m_tri_pool(DEFAULT_POLYGON_TARGET * 3)
	, m_roam(SCENE_SIZE, m_tri_pool)
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();

	std::vector<std::string> textures;
	textures.push_back("u_tex_heightmap");
	textures.push_back("u_tex_detailmap");
	textures.push_back("u_tex_blendmap");
	textures.push_back("u_tex_splat0");
	textures.push_back("u_tex_splat1");
	textures.push_back("u_tex_splat2");

	CU_VEC<ur::VertexAttrib> layout;
	layout.push_back(ur::VertexAttrib("position", 3, 4, 20, 0));
	layout.push_back(ur::VertexAttrib("texcoord", 2, 4, 20, 12));

	shader = std::make_unique<ur::Shader>(&rc, vs, fs, textures, layout);

	size_t vertex_sz = sizeof(float) * (3 + 2);
	size_t max_vertex = SCENE_SIZE * SCENE_SIZE;
	sl::Buffer* buf = new sl::Buffer(vertex_sz, max_vertex);
	vertex_buf = std::make_unique<sl::RenderBuffer>(rc, ur::VERTEXBUFFER, vertex_sz * max_vertex, buf);

	// update mat
	shader->Use();
	shader->SetMat4("u_projection", m_camera.GetProjectionMat().x);
	shader->SetMat4("u_modelview", m_camera.GetModelViewMat().x);

	// callback
	//terr::SplitOnlyROAM::CallbackFuncs cb;
	terr::SplitMergeROAM::CallbackFuncs cb;
	cb.get_height = [&](int x, int y)->uint8_t
	{
		return m_height_map_tex.GetHeight(x, y);
	};
	cb.dis_to_camera = [&](int x, int y)->float
	{
		sm::vec3 p;
		p.x = pos_world2proj(x);
		p.y = m_height_map_tex.GetHeight(x, y) / 255.0f;
		p.z = pos_world2proj(y);
		return sm::dis_pos3_to_pos3(m_camera.GetPos(), p) / POS_TRANS_SCALE;
	};
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
		send_vertex(x, y, SCENE_SIZE);
	};
	m_roam.RegisterCallback(cb);
}

void SimpleSceneApp::Init()
{
	InitTexture();
	m_roam.Init();
}

bool SimpleSceneApp::Update()
{
	m_frustum.CalculateViewFrustum(m_camera);
	return m_roam.Update();
}

void SimpleSceneApp::Draw() const
{
	shader->Use();

	int channel = 0;
	m_height_map_tex.Bind(channel++);
	m_rc->GetUrRc().BindTexture(m_detail_map_tex->TexID(), channel++);
	m_rc->GetUrRc().BindTexture(m_blend_map_tex->TexID(), channel++);
	for (int i = 0; i < 3; ++i) {
		m_rc->GetUrRc().BindTexture(m_splat_tex[i]->TexID(), channel++);
	}

	m_roam.Draw();
	flush_vertex_buf(ur::DRAW_TRIANGLES);
}

void SimpleSceneApp::UpdateModelView()
{
	shader->Use();
	shader->SetMat4("u_modelview", m_camera.GetModelViewMat().x);
	shader->SetVec3("u_cam_pos", m_camera.GetPos().xyz);
}

void SimpleSceneApp::OnKeyDown(rt::KeyType key, int mods)
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

void SimpleSceneApp::InitTexture()
{
	static const int TEX_MAP_SIZE = 512;
	static const float SCALE_NORMAL = 1.0f / 127.0f;
	static const float SCALE_LIGHTS = 1.0f / 255.0f;
	static const int COUNT_LOADED = 3;

	m_height_map_tex.LoadFromImgFile("scene/Height512.tga");

	// texture map
	m_detail_map_tex = terr::TextureLoader::LoadFromRawFile("scene/UnlitColour512.raw", 512, 3);

	// alpha map
	auto h_data = m_height_map_tex.Data();
	auto h_size = m_height_map_tex.Size();
	auto normal_map = terr::TextureGen::CalcNormals(
		h_data, h_size, SCALE, 0.1f, 0.75f, 8);
	auto shadow_map = terr::TextureGen::CalcShadows(
		h_data, h_size, SCALE, LIGHT_DIR);
	assert(normal_map && shadow_map);
	auto light_map = terr::TextureGen::CalcLighting(
		normal_map, shadow_map, h_size, SCALE, 0.5f, 0.5f, LIGHT_DIR);

	// code from lScape by Adrian Welbourn
	// http://www.futurenation.net/glbase/projects.htm
	uint8_t* alpha_map = new uint8_t[4 * TEX_MAP_SIZE * TEX_MAP_SIZE];
	uint8_t* alpha_map_ptr = alpha_map;
	for (int Y = 0; Y < TEX_MAP_SIZE; Y++) {

		/* Scale y into the height map */
		float fhY = (float)Y * (float)h_size / (float)TEX_MAP_SIZE;
		int hY0 = (int)floor(fhY);
		int hY1 = (int)ceil(fhY);

		/* Show progress */
		if (!(Y & 0x3F)) {
			printf(".");
			fflush(stdout);
		}

		for (int X = 0; X < TEX_MAP_SIZE; X++) {
			/* Scale x into the height map */
			float fhX = (float)X * (float)h_size / (float)TEX_MAP_SIZE;
			int hX0 = (int)floor(fhX);
			int hX1 = (int)ceil(fhX);

			/* Get gradient & light, lerping if necessary */
			float grad;
			float light;
			if (hX0 == hX1 && hY0 == hY1)
			{
				grad = terr::TextureGen::NormalMapGet(normal_map, h_size, hX0, hY0)[2];
				light = terr::TextureGen::LightMapGet(light_map, h_size, hX0, hY0);
			}
			else
			{
				float scale;
				float grad00 = terr::TextureGen::NormalMapGet(normal_map, h_size, hX0, hY0)[2];
				float grad01 = terr::TextureGen::NormalMapGet(normal_map, h_size, hX1, hY0)[2];
				float grad10 = terr::TextureGen::NormalMapGet(normal_map, h_size, hX0, hY1)[2];
				float grad11 = terr::TextureGen::NormalMapGet(normal_map, h_size, hX1, hY1)[2];
				float light00 = terr::TextureGen::LightMapGet(light_map, h_size, hX0, hY0);
				float light01 = terr::TextureGen::LightMapGet(light_map, h_size, hX1, hY0);
				float light10 = terr::TextureGen::LightMapGet(light_map, h_size, hX0, hY1);
				float light11 = terr::TextureGen::LightMapGet(light_map, h_size, hX1, hY1);
				float grad0 = grad00;
				float grad1 = grad10;
				float light0 = light00;
				float light1 = light10;
				if (hX0 != hX1) {
					/* Lerp x */
					scale = (float)hX1 - fhX;
					grad0 *= scale;
					grad1 *= scale;
					light0 *= scale;
					light1 *= scale;
					scale = fhX - (float)hX0;
					grad0 += scale * grad01;
					grad1 += scale * grad11;
					light0 += scale * light01;
					light1 += scale * light11;
				}
				grad = grad0;
				light = light0;
				if (hY0 != hY1) {
					/* Lerp y */
					scale = (float)hY1 - fhY;
					grad *= scale;
					light *= scale;
					scale = fhY - (float)hY0;
					grad += scale * grad1;
					light += scale * light1;
				}
			}
			/* Scale grad to range 0 to 1 */
			grad *= SCALE_NORMAL;

			/*
			 * Convert grad from normal.z to true gradient in range 0 to 1
			 */
			grad = 2.0f * acos(grad) / M_PI;
			if (grad < 0.0f)
				grad = 0.0f;
			else if (grad > 1.0f)
				grad = 1.0f;

			/* Scale light to range 0 to 1 */
			light *= SCALE_LIGHTS;
			if (light < 0.0f)
				light = 0.0f;
			else if (light > 1.0f)
				light = 1.0f;

			float fIndex = grad * (float)(COUNT_LOADED - 1);

			int Index0 = (int)floor(fIndex);
			int Index1 = (int)ceil(fIndex);
			if (Index1 == Index0)
				Index1++;
			float scale0 = (float)Index1 - fIndex;
			float scale1 = fIndex - (float)Index0;

			*alpha_map_ptr++ = (uint8_t)(light * 255.0f);
			*alpha_map_ptr++ = (uint8_t)(light * scale0 * 255.0f);
			*alpha_map_ptr++ = (uint8_t)(light * scale1 * 255.0f);
			*alpha_map_ptr++ = 0;
		}
	}
	delete[] normal_map;
	delete[] shadow_map;
	delete[] light_map;

	// todo has multi alpha map, AlphaPerSide != 1
	assert(h_size == TEX_MAP_SIZE);

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	int texid = rc.CreateTexture(alpha_map, TEX_MAP_SIZE, TEX_MAP_SIZE, ur::TEXTURE_RGBA8);
	m_blend_map_tex = std::make_unique<ur::Texture>(&rc, TEX_MAP_SIZE, TEX_MAP_SIZE, ur::TEXTURE_RGBA8, texid);
	delete[] alpha_map;

	for (int i = 0; i < 3; ++i) {
		std::string filename = "scene/Texture" + std::to_string(i) + ".tga";
		m_splat_tex[i] = terr::TextureLoader::LoadFromFile(filename.c_str());
	}
}

}