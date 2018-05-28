#include "Application.h"

#include <shaderlab/ShaderMgr.h>
#include <shaderlab/Shape2Shader.h>
#include <shaderlab/Shape3Shader.h>
#include <shaderlab/Sprite2Shader.h>
#include <shaderlab/Sprite3Shader.h>
#include <shaderlab/BlendShader.h>
#include <shaderlab/FilterShader.h>
#include <shaderlab/MaskShader.h>
#include <shaderlab/Model3Shader.h>
#include <shaderlab/RenderContext.h>
#include <shaderlab/ShaderMgr.h>
#include <facade/RenderContext.h>
#include <facade/Facade.h>

#include <gl/glew.h>
#include <glfw3.h>

namespace
{

void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

}

namespace rt
{

const int Application::WIDTH = 1024;
const int Application::HEIGHT = 768;

Application::Application(const std::string& title)
{
	InitRender();
	InitShader();
	InitSubmodule();
}

Application::~Application()
{
	glfwTerminate();
}

void Application::Flush()
{
	m_rc->GetSlRc().GetShaderMgr().FlushShader();
}

bool Application::InitRender()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "runtime", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	//// Initialize GLEW to setup the OpenGL Function pointers
	//if (glewInit() != GLEW_OK) {
	//	return -1;
	//}

	m_wnd = window;

	m_rc = std::make_shared<facade::RenderContext>();

	return true;
}

bool Application::InitShader()
{
	auto& rc = m_rc->GetSlRc();
	auto& mgr = rc.GetShaderMgr();

	mgr.CreateShader(sl::SHAPE2, new sl::Shape2Shader(rc));
	mgr.CreateShader(sl::SHAPE3, new sl::Shape3Shader(rc));
	mgr.CreateShader(sl::SPRITE2, new sl::Sprite2Shader(rc));
	mgr.CreateShader(sl::SPRITE3, new sl::Sprite3Shader(rc));
	mgr.CreateShader(sl::BLEND, new sl::BlendShader(rc));
	mgr.CreateShader(sl::FILTER, new sl::FilterShader(rc));
	mgr.CreateShader(sl::MASK, new sl::MaskShader(rc));
	mgr.CreateShader(sl::MODEL3, new sl::Model3Shader(rc));

	//ee::DTex::Init();
	//ee::GTxt::Init();

	return true;
}

bool Application::InitSubmodule()
{
	facade::Facade::Init();

	return true;
}

}