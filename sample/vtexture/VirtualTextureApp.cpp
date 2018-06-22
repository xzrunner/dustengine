#include "VirtualTextureApp.h"

#include <painting2/Blackboard.h>
#include <painting2/WindowContext.h>
#include <painting2/PrimitiveDraw.h>
#include <painting2/Color.h>

#include <unirender/Blackboard.h>
#include <unirender/typedef.h>
#include <unirender/RenderContext.h>
#include <unirender/Shader.h>
#include <unirender/RenderContext.h>
#include <shaderlab/RenderBuffer.h>
#include <shaderlab/Blackboard.h>
#include <shaderlab/RenderContext.h>
#include <shaderlab/SubjectMVP2.h>
#include <painting2/DebugDraw.h>
#include <painting3/EffectsManager.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <model/Model.h>
#include <model/Callback.h>
#include <ns/NodeFactory.h>
#include <ns/RegistCallback.h>
#include <ns/Blackboard.h>
#include <node3/RenderSystem.h>
#include <node3/UpdateSystem.h>
#include <node3/CompModel.h>
#include <facade/ResPool.h>
#include <facade/Facade.h>
#include <vtex/feedback.frag>
#include <vtex/final.frag>
#include <vtex/VirtualTexture.h>
#include <vtex/FeedbackBuffer.h>

namespace
{

const char* default_vs = R"(

attribute vec4 position;
attribute vec3 normal;
attribute vec2 texcoord;

uniform mat4 u_projection;
uniform mat4 u_modelview;

varying vec2 v_texcoord;

void main()
{
	gl_Position = u_projection * u_modelview * position;
	v_texcoord = texcoord;
}

)";

uint32_t next_obj_id = 0;

n0::SceneNodePtr node = nullptr;

std::shared_ptr<ur::Shader> feedback_shader = nullptr;
std::shared_ptr<ur::Shader> final_shader = nullptr;

std::unique_ptr<vtex::VirtualTexture> virt_tex = nullptr;
std::unique_ptr<vtex::FeedbackBuffer> feedback_buf = nullptr;

static const int FEEDBACK_SIZE    = 64;
static const int VIRTUAL_TEX_SIZE = 8192;

}

namespace vtexture
{

VirtualTextureApp::VirtualTextureApp()
	: rt::Application3D("VirtualTexture", true)
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();

	CU_VEC<ur::VertexAttrib> layout;
	layout.push_back(ur::VertexAttrib("position", 3, 4));
	layout.push_back(ur::VertexAttrib("normal", 3, 4));
	layout.push_back(ur::VertexAttrib("texcoord", 2, 4));

	vtex::VirtualTextureInfo info(VIRTUAL_TEX_SIZE, 128, 1);
	virt_tex = std::make_unique<vtex::VirtualTexture>("DemoScene_8k.tga.cache", info, 1);

	feedback_buf = std::make_unique<vtex::FeedbackBuffer>(FEEDBACK_SIZE, info.PageTableSize(), virt_tex->GetPageIndexer());

	// feedback
	{
		std::vector<std::string> textures;
		feedback_shader = std::make_shared<ur::Shader>(&rc, default_vs, feedback_frag, textures, layout);

		feedback_shader->Use();

		feedback_shader->SetFloat("u_page_table_size", info.PageTableSize());
		feedback_shader->SetFloat("u_virt_tex_size", VIRTUAL_TEX_SIZE);
		feedback_shader->SetFloat("u_mip_sample_bias", 0);
	}
	// final
	{
		std::vector<std::string> textures;
		textures.push_back("u_page_table_tex");
		textures.push_back("u_texture_atlas_tex");

		final_shader = std::make_shared<ur::Shader>(&rc, default_vs, final_frag, textures, layout);

		final_shader->Use();

		final_shader->SetFloat("u_page_table_size", info.PageTableSize());
		final_shader->SetFloat("u_virt_tex_size", VIRTUAL_TEX_SIZE);
		final_shader->SetFloat("u_atlas_scale", (float)(info.PageSize()) / virt_tex->GetAtlasSize());

		float page_size = info.PageSize();
		final_shader->SetFloat("u_border_scale", (page_size - 2 * info.border_size) / page_size);
		final_shader->SetFloat("u_border_offset", info.border_size / page_size);
	}
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

	node = ns::NodeFactory::Create("DemoScene.XML");
//	node = ns::NodeFactory::Create("C:/Users/gz198/OneDrive/asset/model/solider/meshes/soldier.X");
}

bool VirtualTextureApp::Update()
{
	facade::Facade::Update(0.033f);

	return n3::UpdateSystem::Update(node);
}

void VirtualTextureApp::Draw() const
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();

	// pass 1

	pt3::EffectsManager::Instance()->SetUserEffect(feedback_shader);

	feedback_buf->BindRT();

	feedback_shader->Use();

	auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
	auto screen_sz = wc->GetScreenSize();

	rc.SetViewport(0, 0, FEEDBACK_SIZE, FEEDBACK_SIZE);

	rc.SetDepthFormat(ur::DEPTH_LESS_EQUAL);
	rc.SetClearFlag(ur::MASKC | ur::MASKD);
	rc.Clear(0);

	n3::RenderSystem::Draw(node, m_camera.GetModelViewMat());

	feedback_buf->Download();
	virt_tex->Update(feedback_buf->GetRequests());

	feedback_buf->UnbindRT();

	rc.SetViewport(0, 0, screen_sz.x, screen_sz.y);

	// pass 2
	rc.Clear(0);

	pt3::EffectsManager::Instance()->SetUserEffect(final_shader);
	rc.BindTexture(virt_tex->GetPageTableTexID(), 0);
	rc.BindTexture(virt_tex->GetAtlasTexID(), 1);
	n3::RenderSystem::Draw(node, m_camera.GetModelViewMat());

	// debug
	pt2::DebugDraw::Draw(virt_tex->GetAtlasTexID(), 4);
	pt2::DebugDraw::Draw(feedback_buf->GetTexID(), 3);
	//pt2::DebugDraw::Draw(virt_tex->GetPageTableTexID(), 2);
}

void VirtualTextureApp::UpdateModelView()
{
}

void VirtualTextureApp::OnKeyDown(rt::KeyType key)
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
	}
}

}