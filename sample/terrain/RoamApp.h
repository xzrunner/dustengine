#pragma once

#include "terr/TileMapTex.h"
#include "terr/HeightMapTex.h"
//#include "terr/SplitOnlyROAM.h"
#include "terr/SplitMergeROAM.h"

#include <runtime/Application3D.h>

#include <array>

namespace terrain
{

class RoamApp : public rt::Application3D
{
public:
	RoamApp();

	virtual void Init() override;
	virtual bool Update() override;
	virtual void Draw() const override;

	virtual void UpdateModelView() override;

	virtual void OnKeyDown(rt::KeyType key) override;

private:
	terr::TileMapTex m_tile_map_tex;

	terr::HeightMapTex m_height_map_tex;

	terr::TexturePtr m_detail_map_tex = nullptr;

	//terr::SplitOnlyROAM::BinTriPool m_tri_pool;
	//terr::SplitOnlyROAM m_roam;
	terr::SplitMergeROAM::BinTriPool m_tri_pool;
	terr::SplitMergeROAM m_roam;

}; // RoamApp

}