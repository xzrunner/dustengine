#include "HeightMap.h"

#include <facade/RenderContext.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <unirender/Shader.h>
#include <gimg_import.h>
#include <gimg_typedef.h>
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

static const size_t SIZE = 128;

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

inline float RangedRandom(float f1, float f2)
{
	return (f1 + (f2 - f1)*((float)rand()) / ((float)RAND_MAX));
}

}

namespace terrain
{

HeightMap::HeightMap()
	: rt::Application3D("HeightMap")
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

HeightMap::~HeightMap()
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	if (m_height_map_tex != 0) {
		rc.ReleaseTexture(m_height_map_tex);
	}
	for (auto& tile : m_tile_map_tex) {
		if (tile != 0) {
			rc.ReleaseTexture(tile);
		}
	}
}

void HeightMap::Init()
{
//	m_height_map_tex = LoadHeightMapTex("height128.raw", SIZE);
	m_height_map_tex = GenHeightMapTex(1.0f);

	m_tile_map_tex[LOWEST_TILE]  = LoadTexFromFile("lowestTile.tga");
	m_tile_map_tex[LOW_TILE]     = LoadTexFromFile("lowTile.tga");
	m_tile_map_tex[HIGH_TILE]    = LoadTexFromFile("highTile.tga");
	m_tile_map_tex[HIGHEST_TILE] = LoadTexFromFile("highestTile.tga");

	m_detail_map_tex = LoadTexFromFile("detailMap.tga");
}

void HeightMap::Draw() const
{
	shader->Use();

	m_rc->GetUrRc().BindTexture(m_height_map_tex, 0);
	m_rc->GetUrRc().BindTexture(m_detail_map_tex, 1);
	m_rc->GetUrRc().BindTexture(m_tile_map_tex[0], 2);
	m_rc->GetUrRc().BindTexture(m_tile_map_tex[1], 3);
	m_rc->GetUrRc().BindTexture(m_tile_map_tex[2], 4);
	m_rc->GetUrRc().BindTexture(m_tile_map_tex[3], 5);

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

// from "Focus On 3D Terrain Programming"
//--------------------------------------------------------------
// Name:			CTERRAIN::MakeTerrainPlasma - public
// Description:		Create a height data set using the "Midpoint
//					Displacement" algorithm.  Thanks a lot to
//					Jason Shankel for this code!
//					Note: this algorithm has limited use, since
//					CLOD algorithms usually require a height map
//					size of (n^2)+1 x (n^2)+1, and this algorithm
//					can only generate (n^2) x (n^2) maps
// Arguments:		-iSize: Desired size of the height map
//					-fRoughness: Desired roughness of the created map
// Return Value:	A boolean value: -true: successful creation
//									 -false: unsuccessful creation
//--------------------------------------------------------------
unsigned int HeightMap::GenHeightMapTex(float roughness) const
{
	float* fTempBuffer;
	float fHeight, fHeightReducer;
	int iRectSize = SIZE;
	int ni, nj;
	int mi, mj;
	int pmi, pmj;
	int i, j;
	int x, z;

	if (roughness<0)
		roughness *= -1;

	fHeight = (float)iRectSize / 2;
	fHeightReducer = (float)pow(2, -1 * roughness);

	//allocate the memory for our height data
	auto pixels = new unsigned char[SIZE*SIZE];
	fTempBuffer = new float[SIZE*SIZE];
	if (!pixels || !fTempBuffer) {
		return 0;
	}

	//set the first value in the height field
	fTempBuffer[0] = 0.0f;

	//being the displacement process
	while (iRectSize>0)
	{
		/*Diamond step -

		Find the values at the center of the retangles by averaging the values at
		the corners and adding a random offset:


		a.....b
		.     .
		.  e  .
		.     .
		c.....d

		e  = (a+b+c+d)/4 + random

		In the code below:
		a = (i,j)
		b = (ni,j)
		c = (i,nj)
		d = (ni,nj)
		e = (mi,mj)   */
		for (i = 0; i<SIZE; i += iRectSize)
		{
			for (j = 0; j<SIZE; j += iRectSize)
			{
				ni = (i + iRectSize) % SIZE;
				nj = (j + iRectSize) % SIZE;

				mi = (i + iRectSize / 2);
				mj = (j + iRectSize / 2);

				fTempBuffer[mi + mj * SIZE] = (float)((fTempBuffer[i + j * SIZE] + fTempBuffer[ni + j * SIZE] + fTempBuffer[i + nj * SIZE] + fTempBuffer[ni + nj * SIZE]) / 4 + RangedRandom(-fHeight / 2, fHeight / 2));
			}
		}

		/*Square step -

		Find the values on the left and top sides of each rectangle
		The right and bottom sides are the left and top sides of the neighboring rectangles,
		so we don't need to calculate them

		The height m_heightData.m_ucpData wraps, so we're never left hanging.  The right side of the last
		rectangle in a row is the left side of the first rectangle in the row.  The bottom
		side of the last rectangle in a column is the top side of the first rectangle in
		the column

		.......
		.     .
		.     .
		.  d  .
		.     .
		.     .
		......a..g..b
		.     .     .
		.     .     .
		.  e  h  f  .
		.     .     .
		.     .     .
		......c......

		g = (d+f+a+b)/4 + random
		h = (a+c+e+f)/4 + random

		In the code below:
		a = (i,j)
		b = (ni,j)
		c = (i,nj)
		d = (mi,pmj)
		e = (pmi,mj)
		f = (mi,mj)
		g = (mi,j)
		h = (i,mj)*/
		for (i = 0; i<SIZE; i += iRectSize)
		{
			for (j = 0; j<SIZE; j += iRectSize)
			{

				ni = (i + iRectSize) % SIZE;
				nj = (j + iRectSize) % SIZE;

				mi = (i + iRectSize / 2);
				mj = (j + iRectSize / 2);

				pmi = (i - iRectSize / 2 + SIZE) % SIZE;
				pmj = (j - iRectSize / 2 + SIZE) % SIZE;

				//Calculate the square value for the top side of the rectangle
				fTempBuffer[mi + j * SIZE] = (float)((fTempBuffer[i + j * SIZE] +
					fTempBuffer[ni + j * SIZE] +
					fTempBuffer[mi + pmj * SIZE] +
					fTempBuffer[mi + mj * SIZE]) / 4 +
					RangedRandom(-fHeight / 2, fHeight / 2));

				//Calculate the square value for the left side of the rectangle
				fTempBuffer[i + mj * SIZE] = (float)((fTempBuffer[i + j * SIZE] +
					fTempBuffer[i + nj * SIZE] +
					fTempBuffer[pmi + mj * SIZE] +
					fTempBuffer[mi + mj * SIZE]) / 4 +
					RangedRandom(-fHeight / 2, fHeight / 2));
			}
		}

		//reduce the rectangle size by two to prepare for the next
		//displacement stage
		iRectSize /= 2;

		//reduce the height by the height reducer
		fHeight *= fHeightReducer;
	}

	//normalize the terrain for our purposes
	NormalizeTerrain(fTempBuffer);

	//transfer the terrain into our class's unsigned char height buffer
	for (z = 0; z<SIZE; z++)
	{
		for (x = 0; x < SIZE; x++) {
			pixels[(z * SIZE) + x] = (unsigned char)fTempBuffer[(z*SIZE) + x];
		}
	}

	//delete temporary buffer
	if (fTempBuffer)
	{
		//delete the data
		delete[] fTempBuffer;
	}

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	auto ret = rc.CreateTexture(pixels, SIZE, SIZE, ur::TEXTURE_A8);
	delete[] pixels;

	return ret;
}

unsigned int HeightMap::LoadTexFromFile(const char* filepath)
{
	int width, height, format;
	uint8_t* pixels = gimg_import(filepath, &width, &height, &format);
	assert(pixels);

	ur::TEXTURE_FORMAT tf = ur::TEXTURE_INVALID;
	switch (format)
	{
	case GPF_ALPHA: case GPF_LUMINANCE: case GPF_LUMINANCE_ALPHA:
		tf = ur::TEXTURE_A8;
		break;
	case GPF_RGB:
		tf = ur::TEXTURE_RGB;
		break;
	case GPF_RGBA8:
		tf = ur::TEXTURE_RGBA8;
		break;
	case GPF_BGRA_EXT:
		tf = ur::TEXTURE_BGRA_EXT;
		break;
	case GPF_BGR_EXT:
		tf = ur::TEXTURE_BGR_EXT;
		break;
	case GPF_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		tf = ur::TEXTURE_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case GPF_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		tf = ur::TEXTURE_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case GPF_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		tf = ur::TEXTURE_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		GD_REPORT_ASSERT("unknown type.");
	}

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	auto ret = rc.CreateTexture(pixels, width, height, tf);
	free(pixels);
	return ret;
}

void HeightMap::NormalizeTerrain(float* height_data)
{
	float fMin, fMax;
	float fHeight;
	int i;

	fMin = height_data[0];
	fMax = height_data[0];

	//find the min/max values of the height fTempBuffer
	for (i = 1; i<SIZE*SIZE; i++)
	{
		if (height_data[i]>fMax)
			fMax = height_data[i];

		else if (height_data[i]<fMin)
			fMin = height_data[i];
	}

	//find the range of the altitude
	if (fMax <= fMin)
		return;

	fHeight = fMax - fMin;

	//scale the values to a range of 0-255 (because I like things that way)
	for (i = 0; i<SIZE*SIZE; i++)
		height_data[i] = ((height_data[i] - fMin) / fHeight)*255.0f;
}

}