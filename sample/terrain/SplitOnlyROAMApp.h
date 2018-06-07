#pragma once

#include "terr/TileMapTex.h"
#include "terr/HeightMapTex.h"
#include "terr/BinTriPool.h"
#include "terr/SplitOnlyROAM.h"

#include <runtime/Application3D.h>

#include <array>

namespace terrain
{

class SplitOnlyROAMApp : public rt::Application3D
{
public:
	SplitOnlyROAMApp();

	virtual void Init() override;
	virtual bool Update() override;
	virtual void Draw() const override;

	virtual void UpdateModelView() override;

	virtual void OnKeyDown(rt::KeyType key) override;

private:
	terr::TileMapTex m_tile_map_tex;

	terr::HeightMapTex m_height_map_tex;

	terr::TexturePtr m_detail_map_tex = nullptr;

	terr::BinTriPool    m_tri_pool;
	terr::SplitOnlyROAM m_roam;

}; // SplitOnlyROAMApp

}