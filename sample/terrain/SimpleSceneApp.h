#pragma once

#include <terr/TileMapTex.h>
#include <terr/HeightMapTex.h>
#include <terr/SplitMergeROAM.h>
#include <runtime/Application3D.h>

#include <array>

namespace terrain
{

class SimpleSceneApp : public rt::Application3D
{
public:
	SimpleSceneApp();

	virtual void Init() override;
	virtual bool Update() override;
	virtual void Draw() const override;

	virtual void UpdateModelView() override;

	virtual void OnKeyDown(rt::KeyType key) override;

private:
	void InitTexture();

private:
	struct BlendMapTex
	{
		terr::TexturePtr tex;
	};

private:
	terr::HeightMapTex m_height_map_tex;

	terr::TexturePtr m_detail_map_tex = nullptr;
	terr::TexturePtr m_blend_map_tex  = nullptr;
	terr::TexturePtr m_splat_tex[3];

	terr::SplitMergeROAM::BinTriPool m_tri_pool;
	terr::SplitMergeROAM m_roam;

}; // SimpleSceneApp

}