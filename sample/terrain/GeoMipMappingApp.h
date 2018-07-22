// from "Focus On 3D Terrain Programming"

#pragma once

#include <terr/TileMapTex.h>
#include <terr/HeightMapTex.h>
#include <runtime/Application3D.h>

namespace terrain
{

class GeoMipMappingApp : public rt::Application3D
{
public:
	GeoMipMappingApp(int size, int patch_size);
	virtual ~GeoMipMappingApp();

	virtual void Init() override;
	virtual bool Update() override;
	virtual void Draw() const override;

	virtual void UpdateModelView() override;

	virtual void OnKeyDown(rt::KeyType key, int mods) override;

private:
	struct Patch
	{
		float distance = 0;
		int   lod = 0;
		bool  visible = true;
	};

	struct Neightbor
	{
		bool left = false;
		bool up = false;
		bool right = false;
		bool down = false;
	};

private:
	const Patch& GetPatch(int x, int z) const {
		return m_patches[z * m_num_patch_per_side + x];
	}

	void RenderPatch(int x, int z) const;
	void RenderGrid(float x, float z, float size, const Neightbor& nb) const;

private:
	terr::TileMapTex m_tile_map_tex;

	terr::HeightMapTex m_height_map_tex;

	ur::TexturePtr m_detail_map_tex = nullptr;

	int m_size;

	int m_patch_size;
	int m_num_patch_per_side;
	Patch* m_patches;

	int m_max_lod;

}; // GeoMipMappingApp

}