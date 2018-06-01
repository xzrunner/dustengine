#pragma once

#include "TileMapTex.h"
#include "HeightMapTex.h"

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
	TileMapTex m_tile_map_tex;

	HeightMapTex m_height_map_tex;

	TexturePtr m_detail_map_tex = nullptr;

}; // HeightMapApp

}