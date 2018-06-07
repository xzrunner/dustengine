// continuous real-time optimally adapting mesh
// from "Focus On 3D Terrain Programming"

#pragma once

#include <SM_Vector.h>
#include <runtime/Application3D.h>

namespace terrain
{

class SimpleRoamApp : public rt::Application3D
{
public:
	SimpleRoamApp();

	virtual void Init() override;
	virtual bool Update() override { return false; }
	virtual void Draw() const override;

	virtual void UpdateModelView() override;

private:
	void RenderSub(int iLevel, float* fpVert1, float* fpVert2, float* fpVert3) const;

private:
	float* m_level2dzsize;			//max midpoint displacement per level

	unsigned int m_grid_texid;		//id from glGenTextures
	float m_grid_texcoords[3][3];	//texture coordinates for three verts

	mutable int m_verts_per_frame;	//stat variables
	mutable int m_tris_per_frame;

}; // SimpleRoamApp

}