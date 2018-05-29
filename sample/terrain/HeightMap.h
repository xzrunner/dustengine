#pragma once

#include <runtime/Application3D.h>

namespace terrain
{

class HeightMap : public rt::Application3D
{
public:
	HeightMap();

	virtual void Init() override;
	virtual bool Update() override { return false; }
	virtual void Draw() const override;

	virtual void UpdateModelView() override;

private:
	unsigned int LoadHeightMapTex(const char* filepath, size_t size) const;

}; // HeightMap

}