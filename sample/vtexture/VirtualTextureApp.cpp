#include "VirtualTextureApp.h"

#include <unirender/Blackboard.h>
#include <unirender/typedef.h>
#include <unirender/RenderContext.h>
#include <ns/NodeFactory.h>
#include <ns/RegistCallback.h>
#include <ns/Blackboard.h>
#include <node3/RenderSystem.h>
#include <node3/UpdateSystem.h>
#include <facade/Facade.h>

namespace
{

uint32_t next_obj_id = 0;

static const int FEEDBACK_SIZE    = 64;
static const int VIRTUAL_TEX_SIZE = 8192;

}

namespace vtexture
{

VirtualTextureApp::VirtualTextureApp()
	: rt::Application3D("VirtualTexture", true)
{
	vtex::VirtualTextureInfo info(VIRTUAL_TEX_SIZE, 128, 1);
	m_vt = std::make_unique<vtex::VirtualTexture>("DemoScene_8k.tga.cache", info, 1, FEEDBACK_SIZE, VIRTUAL_TEX_SIZE);
}

void VirtualTextureApp::Init()
{
	facade::Facade::Init();

#ifndef GAME_OBJ_ECS
	ns::RegistCallback::Init();
	ns::Blackboard::Instance()->SetGenNodeIdFunc([]()->uint32_t {
		return next_obj_id++;
	});
#endif // GAME_OBJ_ECS

	m_node = ns::NodeFactory::Create("DemoScene.XML");
//	m_node = ns::NodeFactory::Create("C:/Users/gz198/OneDrive/asset/model/solider/meshes/soldier.X");
}

bool VirtualTextureApp::Update()
{
	facade::Facade::Update(0.033f);

	return n3::UpdateSystem::Update(m_node);
}

void VirtualTextureApp::Draw() const
{
	m_vt->Draw([&]() {
		n3::RenderSystem::Draw(m_node, m_camera.GetModelViewMat());
	});
}

void VirtualTextureApp::UpdateModelView()
{
}

void VirtualTextureApp::OnKeyDown(rt::KeyType key, int mods)
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	switch (key)
	{
	case rt::KEY_W:
		rc.SetPolygonMode(ur::POLYGON_LINE);
		break;
	case rt::KEY_S:
		rc.SetPolygonMode(ur::POLYGON_FILL);
		break;

	case rt::KEY_UP:
		if (mods & rt::KEY_MOD_SHIFT) {
			m_camera.MoveToward(0.1f);
		} else {
			m_camera.MoveToward(1);
		}
		break;
	case rt::KEY_DOWN:
		if (mods & rt::KEY_MOD_SHIFT) {
			m_camera.MoveToward(-0.1f);
		} else {
			m_camera.MoveToward(-1);
		}
		break;

	case rt::KEY_F1:
		m_vt->ClearCache();
		m_vt->GetPageLoader().ChangeShowBorder();
		break;
	case rt::KEY_F2:
		m_vt->ClearCache();
		m_vt->GetPageLoader().ChangeShowMip();
		break;
	}
}

}