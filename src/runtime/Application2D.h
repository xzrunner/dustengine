#pragma once

#include "Application.h"

namespace rt
{

class Application2D : public Application
{
public:
	Application2D(const std::string& title);

private:
	void InitProjection();
	void InitModelView();

}; // Application2D

}