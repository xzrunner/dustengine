#pragma once

#include <unirender/typedef.h>

#include <memory>

namespace terrain
{

class Texture
{
public:
	Texture() {}
	Texture(unsigned int texid, int width, int height,
		ur::TEXTURE_FORMAT format);
	Texture(const char* filepath);
	~Texture();

	int Width() const { return m_width; }
	int Height() const { return m_height; }

	unsigned int GetTexID() const { return m_texid; }

private:
	unsigned int m_texid = 0;

	int m_width = 0, m_height = 0;

	ur::TEXTURE_FORMAT m_format = ur::TEXTURE_INVALID;

}; // Texture

using TexturePtr = std::unique_ptr<Texture>;

}