#include "CROAM.h"

#include <glfw3.h>

int main()
{
	terrain::CROAM app;

	app.Init();

	auto wnd = app.GetWnd();
	while (!glfwWindowShouldClose(wnd))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		app.Update();
		app.Draw();

		app.Flush();

		glfwSwapBuffers(wnd);

		glfwPollEvents();
	}

	return 0;
}