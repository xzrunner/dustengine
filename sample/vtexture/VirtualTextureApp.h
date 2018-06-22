#pragma once

#include <node0/typedef.h>
#include <vtex/VirtualTexture.h>
#include <runtime/Application3D.h>

namespace vtexture
{

class VirtualTextureApp : public rt::Application3D
{
public:
	VirtualTextureApp();

	virtual void Init() override;
	virtual bool Update() override;
	virtual void Draw() const override;

	virtual void UpdateModelView() override;

	virtual void OnKeyDown(rt::KeyType key, int mods) override;

private:
	n0::SceneNodePtr m_node = nullptr;

	std::unique_ptr<vtex::VirtualTexture> m_vt;

}; // VirtualTextureApp

}