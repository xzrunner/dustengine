#include "Texture.h"

#include <guard/check.h>
#include <gimg_import.h>
#include <gimg_typedef.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>

namespace terrain
{

Texture::Texture(unsigned int texid, int width, int height, ur::TEXTURE_FORMAT format)
	: m_texid(texid)
	, m_width(width)
	, m_height(height)
	, m_format(format)
{
}

Texture::Texture(const char* filepath)
{
	int format;
	uint8_t* pixels = gimg_import(filepath, &m_width, &m_height, &format);
	GD_ASSERT(pixels, "load image fail.");

	m_format = ur::TEXTURE_INVALID;
	switch (format)
	{
	case GPF_ALPHA: case GPF_LUMINANCE: case GPF_LUMINANCE_ALPHA:
		m_format = ur::TEXTURE_A8;
		break;
	case GPF_RGB:
		m_format = ur::TEXTURE_RGB;
		break;
	case GPF_RGBA8:
		m_format = ur::TEXTURE_RGBA8;
		break;
	case GPF_BGRA_EXT:
		m_format = ur::TEXTURE_BGRA_EXT;
		break;
	case GPF_BGR_EXT:
		m_format = ur::TEXTURE_BGR_EXT;
		break;
	case GPF_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		m_format = ur::TEXTURE_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case GPF_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		m_format = ur::TEXTURE_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case GPF_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		m_format = ur::TEXTURE_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		GD_REPORT_ASSERT("unknown type.");
	}

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	m_texid = rc.CreateTexture(pixels, m_width, m_height, m_format);
	free(pixels);
}

Texture::~Texture()
{
	if (m_texid != 0) {
		auto& rc = ur::Blackboard::Instance()->GetRenderContext();
		rc.ReleaseTexture(m_texid);
	}
}

}