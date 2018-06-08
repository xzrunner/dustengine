#include "SimpleRoamApp.h"
#include "HeightMapApp.h"
#include "GeoMipMappingApp.h"
#include "RoamApp.h"

#include <glp_loop.h>

#include <glfw3.h>

int main()
{
	//terrain::SimpleRoamApp app;
	//terrain::HeightMapApp app;
	//terrain::GeoMipMappingApp app(513, 17);
	terrain::RoamApp app;

	app.Init();

	glp_loop_init(30);

	auto wnd = app.GetWnd();
	while (!glfwWindowShouldClose(wnd))
	{
		glp_loop_update();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		app.Update();
		app.Draw();

		app.Flush();

		glfwSwapBuffers(wnd);

		glfwPollEvents();
	}

	return 0;
}