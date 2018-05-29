#pragma once

#include "typedef.h"

#include <string>
#include <memory>
#include <functional>

struct GLFWwindow;

namespace facade { class RenderContext; }

namespace rt
{

class EditOP;

class Application
{
public:
	Application(const std::string& title);
	virtual ~Application();

	virtual void Init() = 0;
	virtual bool Update() = 0;
	virtual void Draw() const = 0;

	virtual void UpdateModelView() = 0;

	GLFWwindow* GetWnd() { return m_wnd; }

	EditOP* GetEditOP() { return m_editop.get(); }

	void Flush();

private:
	bool InitRender();
	bool InitShader();
	bool InitSubmodule();

protected:
	static const int WIDTH;
	static const int HEIGHT;

protected:
	std::shared_ptr<facade::RenderContext> m_rc = nullptr;

	std::unique_ptr<EditOP> m_editop = nullptr;

private:
	GLFWwindow* m_wnd = nullptr;

}; // Application

}