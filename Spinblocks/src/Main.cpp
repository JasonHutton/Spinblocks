#include "glad/glad.h"
#define GLFW_INCLUDE_NONE // Just disables the default OpenGL environment explicitly. GLAD should be detected anyway.
#include <GLFW/glfw3.h>
#include <entt/entity/registry.hpp>

// Include GLM core features
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

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

struct position {
	glm::vec2 xy;
};

struct velocity {
	glm::vec2 xy;
};

struct rgba_t {
	glm::vec4 rgba;
	bool enabled;
};

void update(entt::registry& registry) {
	// Views get created when queried. It exposes internal data structures of the registry to itself.
	// Views are cheap to make/destroy.
	// Views are meant to be temporary; don't store them after

	//auto view = registry.view<const position, velocity, rgba_t>();
	auto view = registry.view<rgba_t>();

	/*
	// use a callback
	view.each([](const auto& pos, auto& vel, auto& rgba) { 
	// ...
	});

	// use an extended callback
	view.each([](const auto entity, const auto& pos, auto& vel, auto& rgba) { 
	// ...
	});

	// use a range-for
	for (auto [entity, pos, vel, rgba] : view.each()) {
		// ...
	}

	// use forward iterators and get only the components of interest
	for (auto entity : view)
	{
		auto& vel = view.get<velocity>(entity);
		int q = 0;
		q++;

		//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		// ...
	}
	*/
	
	for (auto entity : view)
	{
		auto& rgba = view.get<rgba_t>(entity);
		if(rgba.enabled)
			glClearColor(rgba.rgba.r, rgba.rgba.g, rgba.rgba.b, rgba.rgba.a);
	}
}

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

	// One time initialization things. Generally before we start to render anything.
	// Create an instance of the Importer class
	Assimp::Importer importer;

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile("./data/box/cube.obj",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (!scene) {
		//DoTheErrorLogging(importer.GetErrorString());
		return -1;
	}
	
	// Begin ECS
	entt::registry registry;

	/* Make 3 entities, each with an rgba_t component. One component is set to be enabled, the others are disabled.
	This causes only one to have any effect, but it triggers in the ECS logic as expected. */
	const auto entity = registry.create();
	registry.emplace<rgba_t>(entity, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), false);
	const auto entity2 = registry.create();
	registry.emplace<rgba_t>(entity2, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), true);
	const auto entity3 = registry.create();
	registry.emplace<rgba_t>(entity3, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), false);
	// End ECS

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);

	// Do one-time OpenGL things here.

	while (!glfwWindowShouldClose(window))
	{
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// Black if glClearColor() is never called.
		//glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Red
		//glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Blue
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Keep running. Generally do stuff
		update(registry);

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