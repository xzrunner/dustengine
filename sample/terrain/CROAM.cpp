#include "CROAM.h"
#include "randtab.h"

#include <facade/RenderContext.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <unirender/Shader.h>
#include <shaderlab/Buffer.h>
#include <shaderlab/RenderBuffer.h>
#include <painting3/PrimitiveDraw.h>

namespace
{

static const int LEVEL_MAX = 30;

const char* vs = R"(

attribute vec4 position;
attribute vec2 texcoord;

uniform mat4 u_projection;
uniform mat4 u_modelview;

varying vec2 v_texcoord;

void main()
{
	gl_Position = u_projection * u_modelview * position;
	v_texcoord = texcoord;
}

)";

const char* fs = R"(

uniform sampler2D u_texture0;

varying vec2 v_texcoord;

void main()
{
	gl_FragColor = texture2D(u_texture0, v_texcoord);
}

)";

struct Vertex
{
	sm::vec3 position;
	sm::vec2 texcoords;
};

std::unique_ptr<ur::Shader>       shader = nullptr;
std::unique_ptr<sl::RenderBuffer> vertex_buf = nullptr;

Vertex vertices[3];

}

namespace terrain
{

CROAM::CROAM()
	: rt::Application3D("CROAM")
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();

	CU_VEC<ur::VertexAttrib> layout;
	layout.push_back(ur::VertexAttrib("position", 3, 4));
	layout.push_back(ur::VertexAttrib("texcoord", 2, 4));

	shader = std::make_unique<ur::Shader>(&rc, vs, fs, layout);

	size_t vertex_sz = sizeof(float) * (3 + 2);
	size_t max_vertex = 4096;
	sl::Buffer* buf = new sl::Buffer(vertex_sz, max_vertex);
	vertex_buf = std::make_unique<sl::RenderBuffer>(rc, ur::VERTEXBUFFER, vertex_sz, max_vertex, buf);

	// update mat
	shader->Use();
	shader->SetMat4("u_projection", m_camera.GetProjectionMat().x);
	shader->SetMat4("u_modelview", m_camera.GetModelViewMat().x);
}

void CROAM::Init()
{
	char cGridTexData[128 * 128 * 4];	//texture for showing mesh

	float a0, a1, a2;
	float f;
	int x, y;
	int k, iLevel;

	m_level2dzsize = new float[LEVEL_MAX + 1];

	//generate table of displacement sizes versus levels
	for (iLevel = 0; iLevel <= LEVEL_MAX; iLevel++)
		m_level2dzsize[iLevel] = 3.0f / ((float)sqrt((float)(1 << iLevel)));

	//generate grid texture
	for (y = 0; y<128; y++)
	{
		for (x = 0; x<128; x++)
		{
			//create bump-shaped function f that is zero on each edge
			a0 = (float)y / 127.0f;
			a1 = (float)(127 - x) / 127.0f;
			a2 = (float)(x - y) / 127.0f;

			f = a0 * a1*a2;
			f = (float)sqrt(fabs(f));

			//quantize the function value and make into color
			//store in rgb components of texture entry
			k = (int)floor(1400.0f*f);

			if (k<0)
				k = 0;
			if (k>255)
				k = 255;

			cGridTexData[4 * (y * 128 + x) + 0] = 30 + (std::sqrt(std::fabs(k))) / 290;
			cGridTexData[4 * (y * 128 + x) + 1] = k;
			cGridTexData[4 * (y * 128 + x) + 2] = (k<128 ? 0 : (k - 128) * 2);
			cGridTexData[4 * (y * 128 + x) + 3] = 0;
		}
	}

	//make texture coordinates for three triangle vertices
	//all triangles use same tex coords when showing grid
	//suck in by half a texel to be correct for bilinear textures
	a0 = 0.5f / 128.0f;
	a1 = 1.0f - a0;

	//set the texture coordinates for three vertices
	vertices[0].texcoords.Set(a0, a0);
	vertices[1].texcoords.Set(a1, a0);
	vertices[2].texcoords.Set(a1, a1);

	//set up the gridview texture
	m_grid_texid = m_rc->GetUrRc().CreateTexture(cGridTexData, 128, 128, ur::TEXTURE_RGBA8);
}

void CROAM::Draw() const
{
	shader->Use();

	float verts[4][3];
	int i;

	//reset the debug counters
	m_verts_per_frame = 0;
	m_tris_per_frame = 0;

	////turn on textured drawing (with the grid texture for now)
	//glBindTexture(GL_TEXTURE_2D, m_grid_texid);
	//glEnable(GL_TEXTURE_2D);

	//render the roam mesh
	//compute four corners of the base square
	for (i = 0; i<4; i++)
	{
		verts[i][0] = ((i & 1) ? 1.0f : -1.0f);
		verts[i][1] = 0.0f;
		verts[i][2] = ((i & 2) ? 1.0f : -1.0f);
	}

	//glColor3f(1.0f, 1.0f, 1.0f);

	//recurse on the two base triangles
	RenderSub(0, verts[0], verts[1], verts[3]);
	RenderSub(0, verts[3], verts[2], verts[0]);

	//end texturing
	//glDisable(GL_TEXTURE_2D);

	// flush
	if (vertex_buf && !vertex_buf->IsEmpty())
	{
		m_rc->GetUrRc().BindTexture(m_grid_texid, 0);
		vertex_buf->Bind();
		vertex_buf->Update();
		ur::Blackboard::Instance()->GetRenderContext().DrawArrays(
			ur::DRAW_TRIANGLES, 0, vertex_buf->Size());
		vertex_buf->Clear();
	}
}

void CROAM::OnKeyPress(rt::KeyType key)
{
	switch (key)
	{
	case rt::KEY_UP:
		m_camera.MoveToward(1);
		shader->Use();
		shader->SetMat4("u_modelview", m_camera.GetModelViewMat().x);
		break;
	case rt::KEY_DOWN:
		m_camera.MoveToward(-1);
		shader->Use();
		shader->SetMat4("u_modelview", m_camera.GetModelViewMat().x);
		break;
	}
}

void CROAM::RenderSub(int iLevel, float* fpVert1, float* fpVert2, float* fpVert3) const
{
	unsigned char* pC;
	unsigned int uiS;
	float fNewVert[3];	// new (split) vertex
	float fSqrEdge, fDistance;
	float fRandHeight;
	int* pInt;
	int  i;

	//squared length of edge (fpVert1-fpVert3)
	fSqrEdge = std::sqrt(std::fabs(fpVert3[0] - fpVert1[0])) +
		       std::sqrt(std::fabs(fpVert3[1] - fpVert1[1])) +
		       std::sqrt(std::fabs(fpVert3[2] - fpVert1[2]));

	//compute split point of base edge
	fNewVert[0] = (fpVert1[0] + fpVert3[0]) / 2.0f;
	fNewVert[1] = 0.0f;
	fNewVert[2] = (fpVert1[2] + fpVert3[2]) / 2.0f;

	//determine random perturbation of center z using hash of x,y
	//random number lookup per byte of (x, z) data, all added
	pC = (unsigned char*)fNewVert;
	for (i = 0, uiS = 0; i<8; i++)
		uiS += randtab[(i << 8) | pC[i]];

	//stuff random hash value bits from uiS into float (float viewed
	//as an int, IEEE float tricks here...)
	pInt = (int*)(&fRandHeight);

	*pInt = 0x40000000 + (uiS & 0x007fffff);
	fRandHeight -= 3.0f;
	fRandHeight = 0.10f*fRandHeight*m_level2dzsize[iLevel];

	//the random height value for the vertex
	fNewVert[1] = ((fpVert1[1] + fpVert3[1]) / 2.0f) + fRandHeight;

	//distance calculation
	auto& cam_pos = m_camera.GetPos();
	fDistance = std::sqrt(std::fabs(fNewVert[0] - cam_pos.x)) +
		        std::sqrt(std::fabs(fNewVert[1] - cam_pos.y)) +
		        std::sqrt(std::fabs(fNewVert[2] - cam_pos.z));

	if (iLevel<LEVEL_MAX && fSqrEdge>fDistance*0.25f)
	{
		//render the children
		RenderSub(iLevel + 1, fpVert1, fNewVert, fpVert2);
		RenderSub(iLevel + 1, fpVert2, fNewVert, fpVert3);

		//the current node doesn't need to be rendered,
		//since both of its children are
		return;
	}

	// add to vertex buffer
	vertices[0].position.Set(fpVert1[0], fpVert1[1], fpVert1[2]);
	vertices[1].position.Set(fpVert2[0], fpVert2[1], fpVert2[2]);
	vertices[2].position.Set(fpVert3[0], fpVert3[1], fpVert3[2]);
	vertex_buf->Add(&vertices[0], 3);

	m_verts_per_frame += 3;
	m_tris_per_frame++;
}

}