#pragma once

#include <runtime/Application3D.h>

#include <array>

namespace terrain
{

class HeightMap : public rt::Application3D
{
public:
	HeightMap();
	virtual ~HeightMap();

	virtual void Init() override;
	virtual bool Update() override { return false; }
	virtual void Draw() const override;

	virtual void UpdateModelView() override;

private:
	unsigned int LoadHeightMapTex(const char* filepath, size_t size) const;
	unsigned int GenHeightMapTex(float roughness) const;

	static unsigned int LoadTexFromFile(const char* filepath);

	// Scale the terrain height values to a range of 0-255
	static void NormalizeTerrain(float* height_data);

private:
	enum TileType
	{
		LOWEST_TILE= 0,		//sand, dirt, etc.
		LOW_TILE,			//grass
		HIGH_TILE,			//mountainside
		HIGHEST_TILE,		//tip of mountain

		MAX_TILE
	};

private:
	unsigned int m_height_map_tex = 0;

	std::array<unsigned int, MAX_TILE> m_tile_map_tex;

	unsigned int m_detail_map_tex = 0;

}; // HeightMap

}