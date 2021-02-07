#include "glad/glad.h"
#define GLFW_INCLUDE_NONE // Just disables the default OpenGL environment explicitly. GLAD should be detected anyway.
#include <GLFW/glfw3.h>
#include <entt/entity/registry.hpp>

#include <string>
#include <iostream>

using std::cout;
using std::endl;

void glfwErrorCallback(int error, const char* description);

struct displayData_t
{
	int x{ 640 };
	int y{ 480 };
	std::string title{ "Spinblocks" };
	//char[] title{ "Spinblocks" };
} displayData;

int main()
{
	if (!glfwInit())
	{
		// Initialization failed.
	}

	glfwSetErrorCallback(glfwErrorCallback);

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	// Might make sense not to explicitly get a context version, for what we're doing?
	// GLFW supports borderless fullscreeen as well. Look later maybe.
	GLFWwindow* window = glfwCreateWindow(displayData.x, displayData.y, displayData.title.c_str(), NULL, NULL);
	if (!window)
	{
		// Window creation or OpenGL Context Creation failed
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);

	// Do one-time OpenGL things here.

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Keep running. Generally do stuff.

		glfwSwapBuffers(window);
		glfwPollEvents(); // Windows needs to do things with the window too!
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void glfwErrorCallback(int error, const char* description)
{
	std::cout << "Error " << error << ": " << description << endl;
}