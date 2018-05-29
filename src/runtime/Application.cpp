#include "Application.h"
#include "EditOP.h"

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	rt::Application* app = static_cast<rt::Application*>(glfwGetWindowUserPointer(window));
	if (!app) {
		return;
	}

	rt::KeyType rt_key = rt::KEY_UNKNOWN;
	switch (key)
	{
	case GLFW_KEY_LEFT:
		rt_key = rt::KEY_LEFT;
		break;
	case GLFW_KEY_RIGHT:
		rt_key = rt::KEY_RIGHT;
		break;
	case GLFW_KEY_UP:
		rt_key = rt::KEY_UP;
		break;
	case GLFW_KEY_DOWN:
		rt_key = rt::KEY_DOWN;
		break;
	}
	if (rt_key == rt::KEY_UNKNOWN) {
		return;
	}

	auto op = app->GetEditOP();
	if (!op) {
		return;
	}

	switch (action)
	{
	case GLFW_PRESS:
	case GLFW_REPEAT:
		op->OnKeyDown(rt_key);
		app->UpdateModelView();
		break;
	case GLFW_RELEASE:
		op->OnKeyUp(rt_key);
		app->UpdateModelView();
		break;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	rt::Application* app = static_cast<rt::Application*>(glfwGetWindowUserPointer(window));
	if (!app) {
		return;
	}
	auto op = app->GetEditOP();
	if (!op) {
		return;
	}

	double x, y;
	glfwGetCursorPos(window, &x, &y);

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS) {
			op->OnMouseLeftDown(float(x), float(y));
			app->UpdateModelView();
		} else if (action == GLFW_RELEASE) {
			op->OnMouseLeftUp(float(x), float(y));
			app->UpdateModelView();
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS) {
			op->OnMouseRightDown(float(x), float(y));
			app->UpdateModelView();
		} else if (action == GLFW_RELEASE) {
			op->OnMouseRightUp(float(x), float(y));
			app->UpdateModelView();
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	rt::Application* app = static_cast<rt::Application*>(glfwGetWindowUserPointer(window));
	if (app && app->GetEditOP())
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);

		app->GetEditOP()->OnMouseWheelRotation(
			float(x), float(y), float(xoffset), float(yoffset));
		app->UpdateModelView();
	}
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	rt::Application* app = static_cast<rt::Application*>(glfwGetWindowUserPointer(window));
	if (!app) {
		return;
	}
	auto op = app->GetEditOP();
	if (!op) {
		return;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ||
		glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		op->OnMouseDrag(float(xpos), float(ypos));
		app->UpdateModelView();
	}
	else
	{
		op->OnMouseMove(float(xpos), float(ypos));
		app->UpdateModelView();
	}
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

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);

	glfwSetWindowUserPointer(window, this);

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