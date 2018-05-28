#include "CROAM.h"
#include "randtab.h"

#include <facade/RenderContext.h>
#include <unirender/RenderContext.h>
#include <painting3/PrimitiveDraw.h>

namespace terrain
{

CROAM::CROAM()
	: rt::Application3D("CROAM")
{
}

void CROAM::Init()
{
	char cGridTexData[128 * 128 * 4];	//texture for showing mesh

	float a0, a1, a2;
	float f;
	int x, y;
	int k, iLevel;

	m_fpLevel2dzsize = new float[LEVEL_MAX + 1];

	//generate table of displacement sizes versus levels
	for (iLevel = 0; iLevel <= LEVEL_MAX; iLevel++)
		m_fpLevel2dzsize[iLevel] = 3.0f / ((float)sqrt((float)(1 << iLevel)));

	//generate grid texture
	for (y = 0; y<128; y++)
	{
		for (x = 0; x<128; x++)
		{
			//create bump-shaped function f that is zero on each edge
			a0 = (float)y / 127.0f;
			a1 = (float)(127 - x) / 127.0f;
			a2 = (float)(x - y) / 127.0f;

			f = a0 * a1*a2;
			f = (float)sqrt(f);

			//quantize the function value and make into color
			//store in rgb components of texture entry
			k = (int)floor(1400.0f*f);

			if (k<0)
				k = 0;
			if (k>255)
				k = 255;

			cGridTexData[4 * (y * 128 + x) + 0] = 30 + (std::sqrt(k)) / 290;
			cGridTexData[4 * (y * 128 + x) + 1] = k;
			cGridTexData[4 * (y * 128 + x) + 2] = (k<128 ? 0 : (k - 128) * 2);
			cGridTexData[4 * (y * 128 + x) + 3] = 0;
		}
	}

	//make texture coordinates for three triangle vertices
	//all triangles use same tex coords when showing grid
	//suck in by half a texel to be correct for bilinear textures
	a0 = 0.5f / 128.0f;
	a1 = 1.0f - a0;

	//set the texture coordinates for three vertices
	m_fGridtex_t[0][0] = a0;	m_fGridtex_t[0][1] = a0;
	m_fGridtex_t[1][0] = a1;	m_fGridtex_t[1][1] = a0;
	m_fGridtex_t[2][0] = a1;	m_fGridtex_t[2][1] = a1;

	//set up the gridview texture
	m_uiGridTexID = m_rc->GetUrRc().CreateTexture(cGridTexData, 128, 128, ur::TEXTURE_RGBA8);
}

void CROAM::Draw() const
{
	float verts[4][3];
	int i;

	//reset the debug counters
	m_iVertsPerFrame = 0;
	m_iTrisPerFrame = 0;

	////turn on textured drawing (with the grid texture for now)
	//glBindTexture(GL_TEXTURE_2D, m_uiGridTexID);
	//glEnable(GL_TEXTURE_2D);

	//render the roam mesh
	//compute four corners of the base square
	for (i = 0; i<4; i++)
	{
		verts[i][0] = ((i & 1) ? 1.0f : -1.0f);
		verts[i][1] = 0.0f;
		verts[i][2] = ((i & 2) ? 1.0f : -1.0f);
	}

	//glColor3f(1.0f, 1.0f, 1.0f);

	//recurse on the two base triangles
	RenderSub(0, verts[0], verts[1], verts[3]);
	RenderSub(0, verts[3], verts[2], verts[0]);

	//end texturing
	//glDisable(GL_TEXTURE_2D);
}

void CROAM::RenderSub(int iLevel, float* fpVert1, float* fpVert2, float* fpVert3) const
{
	unsigned char* pC;
	unsigned int uiS;
	float fNewVert[3];	// new (split) vertex
	float fSqrEdge, fDistance;
	float fRandHeight;
	int* pInt;
	int  i;

	//squared length of edge (fpVert1-fpVert3)
	fSqrEdge = std::sqrt((fpVert3[0] - fpVert1[0])) +
		       std::sqrt((fpVert3[1] - fpVert1[1])) +
		       std::sqrt((fpVert3[2] - fpVert1[2]));

	//compute split point of base edge
	fNewVert[0] = (fpVert1[0] + fpVert3[0]) / 2.0f;
	fNewVert[1] = 0.0f;
	fNewVert[2] = (fpVert1[2] + fpVert3[2]) / 2.0f;

	//determine random perturbation of center z using hash of x,y
	//random number lookup per byte of (x, z) data, all added
	pC = (unsigned char*)fNewVert;
	for (i = 0, uiS = 0; i<8; i++)
		uiS += randtab[(i << 8) | pC[i]];

	//stuff random hash value bits from uiS into float (float viewed
	//as an int, IEEE float tricks here...)
	pInt = (int*)(&fRandHeight);

	*pInt = 0x40000000 + (uiS & 0x007fffff);
	fRandHeight -= 3.0f;
	fRandHeight = 0.10f*fRandHeight*m_fpLevel2dzsize[iLevel];

	//the random height value for the vertex
	fNewVert[1] = ((fpVert1[1] + fpVert3[1]) / 2.0f) + fRandHeight;

	//distance calculation
	fDistance = std::sqrt((fNewVert[0] - m_vecCameraEye[0])) +
		        std::sqrt((fNewVert[1] - m_vecCameraEye[1])) +
		        std::sqrt((fNewVert[2] - m_vecCameraEye[2]));

	if (iLevel<LEVEL_MAX && fSqrEdge>fDistance*0.005f)
	{
		//render the children
		RenderSub(iLevel + 1, fpVert1, fNewVert, fpVert2);
		RenderSub(iLevel + 1, fpVert2, fNewVert, fpVert3);

		//the current node doesn't need to be rendered,
		//since both of its children are
		return;
	}

	//send the vertices to the rendering API

	pt3::PrimitiveDraw::TriLine(fpVert1, fpVert2, fpVert2);

	//glBegin(GL_TRIANGLE_STRIP);
	//	glTexCoord2fv(m_fGridtex_t[0]); glVertex3fv(fpVert1);
	//	glTexCoord2fv(m_fGridtex_t[1]); glVertex3fv(fpVert2);
	//	glTexCoord2fv(m_fGridtex_t[2]); glVertex3fv(fpVert3);
	//glEnd();

	m_iVertsPerFrame += 3;
	m_iTrisPerFrame++;
}

}