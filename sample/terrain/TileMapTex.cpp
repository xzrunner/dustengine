#include "TileMapTex.h"

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>

namespace terrain
{

TileMapTex::TileMapTex()
{
	for (int i = 0; i < MAX_TILE; ++i) {
		m_tile_map_tex[i] = nullptr;
	}
}

void TileMapTex::Init()
{
	m_tile_map_tex[LOWEST_TILE]  = std::make_unique<Texture>("lowestTile.tga");
	m_tile_map_tex[LOW_TILE]     = std::make_unique<Texture>("lowTile.tga");
	m_tile_map_tex[HIGH_TILE]    = std::make_unique<Texture>("highTile.tga");
	m_tile_map_tex[HIGHEST_TILE] = std::make_unique<Texture>("highestTile.tga");
}

void TileMapTex::Bind(int channel_offset) const
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	for (int i = 0; i < MAX_TILE; ++i) {
		rc.BindTexture(m_tile_map_tex[i]->GetTexID(), channel_offset + i);
	}
}

}