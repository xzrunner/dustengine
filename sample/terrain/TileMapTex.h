#pragma once

#include "Texture.h"

#include <array>

namespace terrain
{

class TileMapTex
{
public:
	TileMapTex();

	void Init();

	void Bind(int channel_offset) const;

private:
	enum TileType
	{
		LOWEST_TILE= 0,		//sand, dirt, etc.
		LOW_TILE,			//grass
		HIGH_TILE,			//mountainside
		HIGHEST_TILE,		//tip of mountain

		MAX_TILE
	};

	std::array<TexturePtr, MAX_TILE> m_tile_map_tex;

}; // TileMapTex

}