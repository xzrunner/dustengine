#include "HeightMapTex.h"

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>

#include <fstream>
#include <vector>

namespace
{

inline float RangedRandom(float f1, float f2)
{
	return (f1 + (f2 - f1)*((float)rand()) / ((float)RAND_MAX));
}

}

namespace terrain
{

HeightMapTex::HeightMapTex(bool use_height_info)
	: m_use_height_info(use_height_info)
{
}

HeightMapTex::~HeightMapTex()
{
	if (m_height_info) {
		delete[] m_height_info;
	}
}

void HeightMapTex::LoadFromRawFile(const char* filepath, size_t size)
{
	std::ifstream fin(filepath, std::ios::binary | std::ios::ate);
	if (fin.fail()) {
		return;
	}

	size_t sz = static_cast<size_t>(fin.tellg());
	fin.seekg(0, std::ios::beg);

	auto pixels = new char[size * size];
	if (fin.read(pixels, size * size))
	{
		auto& rc = ur::Blackboard::Instance()->GetRenderContext();
		int texid = rc.CreateTexture(pixels, size, size, ur::TEXTURE_A8);
		m_tex = std::make_unique<Texture>(texid, size, size, ur::TEXTURE_A8);
	}

	if (m_use_height_info) {
		if (m_height_info) {
			delete[] m_height_info;
		}
		m_height_info = (uint8_t*)pixels;
	} else {
		delete[] pixels;
	}
}

// from "Focus On 3D Terrain Programming"
//--------------------------------------------------------------
// Name:			CTERRAIN::MakeTerrainPlasma - public
// Description:		Create a height data set using the "Midpoint
//					Displacement" algorithm.  Thanks a lot to
//					Jason Shankel for this code!
//					Note: this algorithm has limited use, since
//					CLOD algorithms usually require a height map
//					size of (n^2)+1 x (n^2)+1, and this algorithm
//					can only generate (n^2) x (n^2) maps
// Arguments:		-iSize: Desired size of the height map
//					-fRoughness: Desired roughness of the created map
// Return Value:	A boolean value: -true: successful creation
//									 -false: unsuccessful creation
//--------------------------------------------------------------
void HeightMapTex::MakeHeightMapPlasma(int size, float roughness)
{
	float* fTempBuffer;
	float fHeight, fHeightReducer;
	int iRectSize = size;
	int ni, nj;
	int mi, mj;
	int pmi, pmj;
	int i, j;
	int x, z;

	if (roughness<0)
		roughness *= -1;

	fHeight = (float)iRectSize / 2;
	fHeightReducer = (float)pow(2, -1 * roughness);

	//allocate the memory for our height data
	auto pixels = new unsigned char[size*size];
	fTempBuffer = new float[size*size];
	if (!pixels || !fTempBuffer) {
		return;
	}

	//set the first value in the height field
	fTempBuffer[0] = 0.0f;

	//being the displacement process
	while (iRectSize>0)
	{
		/*Diamond step -

		Find the values at the center of the retangles by averaging the values at
		the corners and adding a random offset:


		a.....b
		.     .
		.  e  .
		.     .
		c.....d

		e  = (a+b+c+d)/4 + random

		In the code below:
		a = (i,j)
		b = (ni,j)
		c = (i,nj)
		d = (ni,nj)
		e = (mi,mj)   */
		for (i = 0; i<size; i += iRectSize)
		{
			for (j = 0; j<size; j += iRectSize)
			{
				ni = (i + iRectSize) % size;
				nj = (j + iRectSize) % size;

				mi = (i + iRectSize / 2);
				mj = (j + iRectSize / 2);

				fTempBuffer[mi + mj * size] = (float)((fTempBuffer[i + j * size] + fTempBuffer[ni + j * size] + fTempBuffer[i + nj * size] + fTempBuffer[ni + nj * size]) / 4 + RangedRandom(-fHeight / 2, fHeight / 2));
			}
		}

		/*Square step -

		Find the values on the left and top sides of each rectangle
		The right and bottom sides are the left and top sides of the neighboring rectangles,
		so we don't need to calculate them

		The height m_heightData.m_ucpData wraps, so we're never left hanging.  The right side of the last
		rectangle in a row is the left side of the first rectangle in the row.  The bottom
		side of the last rectangle in a column is the top side of the first rectangle in
		the column

		.......
		.     .
		.     .
		.  d  .
		.     .
		.     .
		......a..g..b
		.     .     .
		.     .     .
		.  e  h  f  .
		.     .     .
		.     .     .
		......c......

		g = (d+f+a+b)/4 + random
		h = (a+c+e+f)/4 + random

		In the code below:
		a = (i,j)
		b = (ni,j)
		c = (i,nj)
		d = (mi,pmj)
		e = (pmi,mj)
		f = (mi,mj)
		g = (mi,j)
		h = (i,mj)*/
		for (i = 0; i<size; i += iRectSize)
		{
			for (j = 0; j<size; j += iRectSize)
			{

				ni = (i + iRectSize) % size;
				nj = (j + iRectSize) % size;

				mi = (i + iRectSize / 2);
				mj = (j + iRectSize / 2);

				pmi = (i - iRectSize / 2 + size) % size;
				pmj = (j - iRectSize / 2 + size) % size;

				//Calculate the square value for the top side of the rectangle
				fTempBuffer[mi + j * size] = (float)((fTempBuffer[i + j * size] +
					fTempBuffer[ni + j * size] +
					fTempBuffer[mi + pmj * size] +
					fTempBuffer[mi + mj * size]) / 4 +
					RangedRandom(-fHeight / 2, fHeight / 2));

				//Calculate the square value for the left side of the rectangle
				fTempBuffer[i + mj * size] = (float)((fTempBuffer[i + j * size] +
					fTempBuffer[i + nj * size] +
					fTempBuffer[pmi + mj * size] +
					fTempBuffer[mi + mj * size]) / 4 +
					RangedRandom(-fHeight / 2, fHeight / 2));
			}
		}

		//reduce the rectangle size by two to prepare for the next
		//displacement stage
		iRectSize /= 2;

		//reduce the height by the height reducer
		fHeight *= fHeightReducer;
	}

	//normalize the terrain for our purposes
	NormalizeTerrain(size, fTempBuffer);

	//transfer the terrain into our class's unsigned char height buffer
	for (z = 0; z<size; z++)
	{
		for (x = 0; x < size; x++) {
			pixels[(z * size) + x] = (unsigned char)fTempBuffer[(z*size) + x];
		}
	}

	//delete temporary buffer
	if (fTempBuffer)
	{
		//delete the data
		delete[] fTempBuffer;
	}

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	int texid = rc.CreateTexture(pixels, size, size, ur::TEXTURE_A8);

	m_tex = std::make_unique<Texture>(texid, size, size, ur::TEXTURE_A8);

	if (m_use_height_info) {
		if (m_height_info) {
			delete[] m_height_info;
		}
		m_height_info = (uint8_t*)pixels;
	} else {
		delete[] pixels;
	}
}

void HeightMapTex::MakeHeightMapFault(int size, int iIterations, int iMinDelta, int iMaxDelta, float fFilter)
{
	float* fTempBuffer;
	int iCurrentIteration;
	int iHeight;
	int iRandX1, iRandZ1;
	int iRandX2, iRandZ2;
	int iDirX1, iDirZ1;
	int iDirX2, iDirZ2;
	int x, z;
	int i;

	//allocate the memory for our height data
	auto pixels = new unsigned char[size*size];
	fTempBuffer = new float[size*size];
	if (!pixels || !fTempBuffer) {
		return;
	}

	//clear the height fTempBuffer
	for (i = 0; i<size*size; i++)
		fTempBuffer[i] = 0;

	for (iCurrentIteration = 0; iCurrentIteration<iIterations; iCurrentIteration++)
	{
		//calculate the height range (linear interpolation from iMaxDelta to
		//iMinDelta) for this fault-pass
		iHeight = iMaxDelta - ((iMaxDelta - iMinDelta)*iCurrentIteration) / iIterations;

		//pick two points at random from the entire height map
		iRandX1 = rand() % size;
		iRandZ1 = rand() % size;

		//check to make sure that the points are not the same
		do
		{
			iRandX2 = rand() % size;
			iRandZ2 = rand() % size;
		} while (iRandX2 == iRandX1 && iRandZ2 == iRandZ1);


		//iDirX1, iDirZ1 is a vector going the same direction as the line
		iDirX1 = iRandX2 - iRandX1;
		iDirZ1 = iRandZ2 - iRandZ1;

		for (z = 0; z<size; z++)
		{
			for (x = 0; x<size; x++)
			{
				//iDirX2, iDirZ2 is a vector from iRandX1, iRandZ1 to the current point (in the loop)
				iDirX2 = x - iRandX1;
				iDirZ2 = z - iRandZ1;

				//if the result of ( iDirX2*iDirZ1 - iDirX1*iDirZ2 ) is "up" (above 0),
				//then raise this point by iHeight
				if ((iDirX2*iDirZ1 - iDirX1 * iDirZ2)>0)
					fTempBuffer[(z*size) + x] += (float)iHeight;
			}
		}

		//erode terrain
		FilterHeightField(size, fTempBuffer, fFilter);
	}

	//normalize the terrain for our purposes
	NormalizeTerrain(size, fTempBuffer);

	//transfer the terrain into our class's unsigned char height buffer
	for (z = 0; z<size; z++) {
		for (x = 0; x < size; x++) {
			pixels[(z * size) + x] = (unsigned char)fTempBuffer[(z*size) + x];
		}
	}

	//delete temporary buffer
	if (fTempBuffer)
	{
		//delete the data
		delete[] fTempBuffer;
	}

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	int texid = rc.CreateTexture(pixels, size, size, ur::TEXTURE_A8);

	m_tex = std::make_unique<Texture>(texid, size, size, ur::TEXTURE_A8);

	if (m_use_height_info) {
		if (m_height_info) {
			delete[] m_height_info;
		}
		m_height_info = (uint8_t*)pixels;
	} else {
		delete[] pixels;
	}

}

void HeightMapTex::Bind(int channel) const
{
	if (m_tex)
	{
		auto& rc = ur::Blackboard::Instance()->GetRenderContext();
		rc.BindTexture(m_tex->GetTexID(), channel);
	}
}

int HeightMapTex::GetHeight(int x, int z) const
{
	if (m_height_info) {
		return m_height_info[z * m_tex->Width() + x];
	} else {
		return 0;
	}
}

void HeightMapTex::NormalizeTerrain(int size, float* height_data)
{
	float fMin, fMax;
	float fHeight;
	int i;

	fMin = height_data[0];
	fMax = height_data[0];

	//find the min/max values of the height fTempBuffer
	for (i = 1; i<size*size; i++)
	{
		if (height_data[i]>fMax)
			fMax = height_data[i];

		else if (height_data[i]<fMin)
			fMin = height_data[i];
	}

	//find the range of the altitude
	if (fMax <= fMin)
		return;

	fHeight = fMax - fMin;

	//scale the values to a range of 0-255 (because I like things that way)
	for (i = 0; i<size*size; i++)
		height_data[i] = ((height_data[i] - fMin) / fHeight)*255.0f;
}

//--------------------------------------------------------------
// Name:			CTERRAIN::FilterHeightBand - private
// Description:		Apply the erosion filter to an individual
//					band of height values
// Arguments:		-fpBand: the band to be filtered
//					-iStride: how far to advance per pass
//					-iCount: Number of passes to make
//					-fFilter: the filter strength
// Return Value:	None
//--------------------------------------------------------------
void HeightMapTex::FilterHeightBand(float* fpBand, int iStride, int iCount, float fFilter)
{
	float v = fpBand[0];
	int j = iStride;
	int i;

	//go through the height band and apply the erosion filter
	for (i = 0; i<iCount - 1; i++)
	{
		fpBand[j] = fFilter * v + (1 - fFilter)*fpBand[j];

		v = fpBand[j];
		j += iStride;
	}
}

//--------------------------------------------------------------
// Name:			CTERRAIN::FilterHeightfTempBuffer - private
// Description:		Apply the erosion filter to an entire buffer
//					of height values
// Arguments:		-fpHeightData: the height values to be filtered
//					-fFilter: the filter strength
// Return Value:	None
//--------------------------------------------------------------
void HeightMapTex::FilterHeightField(int size, float* fpHeightData, float fFilter)
{
	int i;

	//erode left to right
	for (i = 0; i<size; i++)
		FilterHeightBand(&fpHeightData[size*i], 1, size, fFilter);

	//erode right to left
	for (i = 0; i<size; i++)
		FilterHeightBand(&fpHeightData[size*i + size - 1], -1, size, fFilter);

	//erode top to bottom
	for (i = 0; i<size; i++)
		FilterHeightBand(&fpHeightData[i], size, size, fFilter);

	//erode from bottom to top
	for (i = 0; i<size; i++)
		FilterHeightBand(&fpHeightData[size*(size - 1) + i], -size, size, fFilter);
}

}