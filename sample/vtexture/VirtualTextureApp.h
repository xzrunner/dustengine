#pragma once

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

	virtual void OnKeyDown(rt::KeyType key) override;

}; // VirtualTextureApp

}