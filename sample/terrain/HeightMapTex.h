#pragma once

#include "Texture.h"

#include <stdint.h>

namespace terrain
{

class HeightMapTex
{
public:
	HeightMapTex(bool use_height_info = false);
	~HeightMapTex();

	void LoadFromRawFile(const char* filepath, size_t size);

	// gen
	void MakeHeightMapPlasma(int size, float roughness);
	void MakeHeightMapFault(int size, int iIterations, int iMinDelta, int iMaxDelta, float fFilter);

	void Bind(int channel) const;

	int GetHeight(int x, int z) const;

private:
	// Scale the terrain height values to a range of 0-255
	static void NormalizeTerrain(int size, float* height_data);

	static void FilterHeightBand(float* fpBand, int iStride, int iCount, float fFilter);
	static void FilterHeightField(int sizef, float* fpHeightData, float fFilter);

private:
	bool m_use_height_info = false;
	uint8_t* m_height_info = nullptr;

	TexturePtr m_tex = nullptr;

}; // HeightMapTex

}