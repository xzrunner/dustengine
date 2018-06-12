#pragma once

//#define SPLIT_ONLY
#define SPLIT_MERGE
//#define SPLIT_MERGE_QUEUE

#include <terr/TileMapTex.h>
#include <terr/HeightMapTex.h>
#ifdef SPLIT_ONLY
#include <terr/SplitOnlyROAM.h>
#elif defined SPLIT_MERGE
#include <terr/SplitMergeROAM.h>
#elif defined SPLIT_MERGE_QUEUE
#include <terr/SplitMergeQueueROAM.h>
#endif

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

#ifdef SPLIT_ONLY
	terr::SplitOnlyROAM::BinTriPool m_tri_pool;
	terr::SplitOnlyROAM m_roam;
#elif defined SPLIT_MERGE
	terr::SplitMergeROAM::BinTriPool m_tri_pool;
	terr::SplitMergeROAM m_roam;
#elif defined SPLIT_MERGE_QUEUE
	terr::SplitMergeQueueROAM::BinTriPool m_tri_pool;
	terr::SplitMergeQueueROAM m_roam;
#endif

}; // RoamApp

}