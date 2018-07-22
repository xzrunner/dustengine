#pragma once

#include <terr/TileMapTex.h>
#include <terr/HeightMapTex.h>
#include <runtime/Application3D.h>

#include <array>

namespace terrain
{

class HeightMapApp : public rt::Application3D
{
public:
	HeightMapApp();

	virtual void Init() override;
	virtual bool Update() override { return false; }
	virtual void Draw() const override;

	virtual void UpdateModelView() override;

private:
	terr::TileMapTex m_tile_map_tex;

	terr::HeightMapTex m_height_map_tex;

	ur::TexturePtr m_detail_map_tex = nullptr;

}; // HeightMapApp

}