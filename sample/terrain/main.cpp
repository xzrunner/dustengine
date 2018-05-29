#include "CROAM.h"
#include "HeightMap.h"

#include <glp_loop.h>

#include <glfw3.h>

int main()
{
	//terrain::CROAM app;
	terrain::HeightMap app;

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