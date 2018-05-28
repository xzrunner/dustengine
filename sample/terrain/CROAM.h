// continuous real-time optimally adapting mesh
// from "Focus On 3D Terrain Programming"

#pragma once

#include <SM_Vector.h>
#include <runtime/Application3D.h>

namespace terrain
{

class CROAM : public rt::Application3D
{
public:
	CROAM();

	virtual void Init() override;
	virtual bool Update() override { return false; }
	virtual void Draw() const override;

private:
	void RenderSub(int iLevel, float* fpVert1, float* fpVert2, float* fpVert3) const;

private:
	static const int LEVEL_MAX = 30;

private:
	sm::vec3 m_vecCameraEye;

	float* m_fpLevel2dzsize;			//max midpoint displacement per level

	unsigned int m_uiGridTexID;			//id from glGenTextures
	float m_fGridtex_t[3][3];			//texture coordinates for three verts

	mutable int m_iVertsPerFrame;		//stat variables
	mutable int m_iTrisPerFrame;

}; // CROAM

}