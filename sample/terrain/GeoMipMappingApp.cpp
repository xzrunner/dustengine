#include "GeoMipMappingApp.h"

#include <SM_Calc.h>
#include <facade/RenderContext.h>
#include <unirender/Shader.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <shaderlab/Buffer.h>
#include <shaderlab/RenderBuffer.h>
#include <painting3/PrimitiveDraw.h>

#define BATCH_VERTICES

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

const float POS_TRANS_OFF = 1.0f;
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

// profile
int LOD_COUNT[4];

}

namespace terrain
{

GeoMipMappingApp::GeoMipMappingApp(int size, int patch_size)
	: rt::Application3D("GeoMipMappingApp")
	, m_size(size)
	, m_patch_size(patch_size)
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
	size_t max_vertex = 1024 * 100;
	sl::Buffer* buf = new sl::Buffer(vertex_sz, max_vertex);
	vertex_buf = std::make_unique<sl::RenderBuffer>(rc, ur::VERTEXBUFFER, vertex_sz, max_vertex, buf);

	// update mat
	shader->Use();
	shader->SetMat4("u_projection", m_camera.GetProjectionMat().x);
	shader->SetMat4("u_modelview", m_camera.GetModelViewMat().x);
}

GeoMipMappingApp::~GeoMipMappingApp()
{
	if (m_patches) {
		delete[] m_patches;
	}
}

void GeoMipMappingApp::Init()
{
	m_height_map_tex.MakeHeightMapFault(m_size, 64, 0, 255, 0.05f);
	m_tile_map_tex.Init();
	m_detail_map_tex = std::make_unique<terr::Texture>("detailMap.tga");

	if (m_patches) {
		delete[] m_patches;
	}

	m_num_patch_per_side = m_size / m_patch_size;
	int patches_num = std::pow(m_num_patch_per_side, 2);
	m_patches = new Patch[patches_num];
	if (!m_patches) {
		return;
	}

	int divisor = m_patch_size - 1;
	int lod = 0;
	while (divisor>2) {
		divisor = divisor >> 1;
		lod++;
	}

	m_max_lod = lod;

	for (int i = 0; i < patches_num; ++i) {
		m_patches[i].lod = m_max_lod;
		m_patches[i].visible = true;
	}
}

bool GeoMipMappingApp::Update()
{
	for (int iz = 0; iz < m_num_patch_per_side; ++iz) {
		for (int ix = 0; ix < m_num_patch_per_side; ++ix) {
			auto& patch = const_cast<Patch&>(GetPatch(ix, iz));
			int ipatch = iz * m_num_patch_per_side + ix;

			float x = m_patch_size * (ix + 0.5f);
			float z = m_patch_size * (iz + 0.5f);
			float y = m_height_map_tex.GetHeight(x, z);
			x = pos_world2proj(x);
			y = y / 255.0f;
			z = pos_world2proj(z);

			// culling
			if (m_frustum.CubeFrustumTest(x, y, z, m_patch_size * POS_TRANS_SCALE)) {
				m_patches[ipatch].visible = true;
			} else {
				m_patches[ipatch].visible = false;
			}

			if (patch.visible)
			{
				patch.distance = sm::dis_pos3_to_pos3(m_camera.GetPos(), sm::vec3(x, y, z));
				if (patch.distance < 0.5f) {
					patch.lod = 0;
					++LOD_COUNT[0];
				} else if (patch.distance < 1) {
					patch.lod = 1;
					++LOD_COUNT[1];
				} else if (patch.distance < 2.5f) {
					patch.lod = 2;
					++LOD_COUNT[2];
				} else {
					patch.lod = 3;
					++LOD_COUNT[3];
				}
			}
		}
	}

	return true;
}

void GeoMipMappingApp::Draw() const
{
	m_frustum.CalculateViewFrustum(m_camera);

	shader->Use();

	m_height_map_tex.Bind(0);
	m_rc->GetUrRc().BindTexture(m_detail_map_tex->GetTexID(), 1);
	m_tile_map_tex.Bind(2);

	for (int iz = 0; iz < m_num_patch_per_side; ++iz) {
		for (int ix = 0; ix < m_num_patch_per_side; ++ix) {
			auto& patch = const_cast<Patch&>(GetPatch(ix, iz));
			if (patch.visible) {
				RenderPatch(ix, iz);
			}
		}
	}

#ifdef BATCH_VERTICES
	flush_vertex_buf(ur::DRAW_TRIANGLE_STRIP);
#endif // BATCH_VERTICES

	// profile
	for (int i = 0; i < 4; ++i) {
		LOD_COUNT[i] = 0;
	}
}

void GeoMipMappingApp::UpdateModelView()
{
	shader->Use();
	shader->SetMat4("u_modelview", m_camera.GetModelViewMat().x);
}

void GeoMipMappingApp::OnKeyDown(rt::KeyType key)
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

void GeoMipMappingApp::RenderPatch(int px, int pz) const
{
	Neightbor patch_nb;
	Neightbor fan_nb;

	auto& patch = GetPatch(px, pz);

	// left
	if (px == 0 || GetPatch(px - 1, pz).lod <= patch.lod) {
		patch_nb.left = true;
	} else {
		patch_nb.left = false;
	}
	// right
	if (px == m_num_patch_per_side - 1 || GetPatch(px + 1, pz).lod <= patch.lod) {
		patch_nb.right = true;
	} else {
		patch_nb.right = false;
	}
	// lower
	if (pz == 0 || GetPatch(px, pz - 1).lod <= patch.lod) {
		patch_nb.down = true;
	} else {
		patch_nb.down = false;
	}
	// upper
	if (pz == m_num_patch_per_side - 1 || GetPatch(px, pz + 1).lod <= patch.lod) {
		patch_nb.up = true;
	} else {
		patch_nb.up = false;
	}

	int lod = patch.lod;
	float size = m_patch_size;
	int divisor = m_patch_size - 1;

	//find out how many fan divisions we are going to have
	while (--lod > -1) {
		divisor = divisor >> 1;
	}

	//the size between the center of each triangle fan
	size /= divisor;

	float half_size = size / 2.0f;
	for (float z = half_size; z + half_size < m_patch_size + 1; z += size) {
		for (float x = half_size; x + half_size < m_patch_size + 1; x += size) {
			// left
			if (x == half_size) {
				fan_nb.left = patch_nb.left;
			} else {
				fan_nb.left = true;
			}
			// right
			if (x >= m_patch_size - half_size) {
				fan_nb.right = patch_nb.right;
			} else {
				fan_nb.right = true;
			}
			// lower
			if (z == half_size) {
				fan_nb.down = patch_nb.down;
			} else {
				fan_nb.down = true;
			}
			// upper
			if (z >= m_patch_size - half_size) {
				fan_nb.up = patch_nb.up;
			} else {
				fan_nb.up = true;
			}

			RenderGrid(px * m_patch_size + x, pz * m_patch_size + z, size, fan_nb);
		}
	}
}

#ifdef BATCH_VERTICES

void GeoMipMappingApp::RenderGrid(float x, float z, float size, const Neightbor& nb) const
{
	float hsz = size / 2.0f;

	int count = 0;

	// center
	vertex_buf->Add(&Vertex(x, z, m_size), 1);
	// copy first
	vertex_buf->Add(&Vertex(x, z, m_size), 1);

	if (nb.left)
	{
		vertex_buf->Add(&Vertex(x - hsz, z, m_size), 1);
		++count;
		if (count == 2) {
			vertex_buf->Add(&Vertex(x, z, m_size), 1);
			count = 0;
		}
	}
	vertex_buf->Add(&Vertex(x - hsz, z + hsz, m_size), 1);
	++count;
	if (count == 2) {
		vertex_buf->Add(&Vertex(x, z, m_size), 1);
		count = 0;
	}
	if (nb.up)
	{
		vertex_buf->Add(&Vertex(x, z + hsz, m_size), 1);
		++count;
		if (count == 2) {
			vertex_buf->Add(&Vertex(x, z, m_size), 1);
			count = 0;
		}
	}
	vertex_buf->Add(&Vertex(x + hsz, z + hsz, m_size), 1);
	++count;
	if (count == 2) {
		vertex_buf->Add(&Vertex(x, z, m_size), 1);
		count = 0;
	}
	if (nb.right)
	{
		vertex_buf->Add(&Vertex(x + hsz, z, m_size), 1);
		++count;
		if (count == 2) {
			vertex_buf->Add(&Vertex(x, z, m_size), 1);
			count = 0;
		}
	}
	vertex_buf->Add(&Vertex(x + hsz, z - hsz, m_size), 1);
	++count;
	if (count == 2) {
		vertex_buf->Add(&Vertex(x, z, m_size), 1);
		count = 0;
	}
	if (nb.down)
	{
		vertex_buf->Add(&Vertex(x, z - hsz, m_size), 1);
		++count;
		if (count == 2) {
			vertex_buf->Add(&Vertex(x, z, m_size), 1);
			count = 0;
		}
	}
	Vertex last(x - hsz, z - hsz, m_size);
	vertex_buf->Add(&Vertex(x - hsz, z - hsz, m_size), 1);
	++count;
	if (count == 2) {
		last = Vertex(x, z, m_size);
		vertex_buf->Add(&Vertex(x, z, m_size), 1);
		count = 0;
	}
	if (nb.left)
	{
		last = Vertex(x - hsz, z, m_size);
		vertex_buf->Add(&Vertex(x - hsz, z, m_size), 1);
		++count;
		if (count == 2) {
			last = Vertex(x, z, m_size);
			vertex_buf->Add(&Vertex(x, z, m_size), 1);
			count = 0;
		}
	}
	else
	{
		last = Vertex(x - hsz, z + hsz, m_size);
		vertex_buf->Add(&Vertex(x - hsz, z + hsz, m_size), 1);
		++count;
		if (count == 2) {
			last = Vertex(x, z, m_size);
			vertex_buf->Add(&Vertex(x, z, m_size), 1);
			count = 0;
		}
	}
	// last
	vertex_buf->Add(&last, 1);
}

#else

void GeoMipMappingApp::RenderGrid(float x, float z, float size, const Neightbor& nb) const
{
	vertex_buf->Clear();

	float hsz = size / 2.0f;

	vertex_buf->Add(&Vertex(x, z, m_size), 1);
	if (nb.left) {
		vertex_buf->Add(&Vertex(x - hsz, z, m_size), 1);
	}
	vertex_buf->Add(&Vertex(x - hsz, z + hsz, m_size), 1);
	if (nb.up) {
		vertex_buf->Add(&Vertex(x, z + hsz, m_size), 1);
	}
	vertex_buf->Add(&Vertex(x + hsz, z + hsz, m_size), 1);
	if (nb.right) {
		vertex_buf->Add(&Vertex(x + hsz, z, m_size), 1);
	}
	vertex_buf->Add(&Vertex(x + hsz, z - hsz, m_size), 1);
	if (nb.down) {
		vertex_buf->Add(&Vertex(x, z - hsz, m_size), 1);
	}
	vertex_buf->Add(&Vertex(x - hsz, z - hsz, m_size), 1);
	if (nb.left) {
		vertex_buf->Add(&Vertex(x - hsz, z, m_size), 1);
	} else {
		vertex_buf->Add(&Vertex(x - hsz, z + hsz, m_size), 1);
	}

	vertex_buf->Bind();
	vertex_buf->Update();
	ur::Blackboard::Instance()->GetRenderContext().DrawArrays(
		ur::DRAW_TRIANGLE_FAN, 0, vertex_buf->Size());
}

#endif // BATCH_VERTICES

}