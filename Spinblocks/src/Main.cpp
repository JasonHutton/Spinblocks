#include "glad/glad.h"
#define GLFW_INCLUDE_NONE // Just disables the default OpenGL environment explicitly. GLAD should be detected anyway.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <entt/entity/registry.hpp>

// Include GLM core features
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/shader.h>
#include <learnopengl/mesh.h>
#include <learnopengl/model.h>
#include <learnopengl/camera.h>

#include <string>
#include <iostream>
#include <vector>

#include "Components/Includes.h"
#include "GameTime.h""

using std::string;
using std::cout;
using std::endl;
using std::vector;

template<class T>
T* Coalesce(T* value, T* defaultValue)
{
	return value != NULL ? value : defaultValue;
}

void glfwErrorCallback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
auto shaders = std::unordered_map<std::string, Shader*>();

Shader* RetrieveShader(const char* key, const char* vs, const char* fs)
{
	if (shaders.count(key) > 0)
	{
		return shaders[key];
	}
	else
	{
		return (shaders[key] = new Shader(vs, fs));
	}
}

struct displayData_t
{
	int x{ 640 };
	int y{ 480 };
	std::string title{ "Spinblocks" };
} displayData;


//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void update(entt::registry& registry) {
	// Views get created when queried. It exposes internal data structures of the registry to itself.
	// Views are cheap to make/destroy.
	// Views are meant to be temporary; don't store them after

	/*auto gameObjectView = registry.view<GameObjectComponent>();
	for (auto entity : gameObjectView)
	{
		auto& gameObject = gameObjectView.get<GameObjectComponent>(entity);
		if (gameObject.IsEnabled())
		{
			
		}
	}*/
}

void render(entt::registry& registry)
{
	// Views get created when queried. It exposes internal data structures of the registry to itself.
	// Views are cheap to make/destroy.
	// Views are meant to be temporary; don't store them after


	Shader* shader = shaders["model"]; // Need to look at this a bit closer.

	glm::mat4 projectionMatrix = glm::mat4(1.0f); // Identity Matrix
	glm::mat4 viewMatrix = glm::mat4(1.0f); // Identity Matrix

	// We're assuming we just have one here, and that it's always enabled, even though we're checking for it.
	auto cameraView = registry.view<Components::Camera>();
	for (auto entity : cameraView)
	{
		auto& camera = cameraView.get<Components::Camera>(entity);
		if (camera.IsEnabled())
		{
			projectionMatrix = glm::perspective(glm::radians(camera.m_camera.Zoom), (float)displayData.x / (float)displayData.y, 0.1f, 100.0f);
			viewMatrix = camera.m_camera.GetViewMatrix();

			shader->use();
			shader->setMat4("projection", projectionMatrix);
			shader->setMat4("view", viewMatrix);
		}
	}


	auto renderView = registry.view<Components::Renderable, Components::Position, Components::Scale>();
	for (auto entity : renderView)
	{
		auto& render = renderView.get<Components::Renderable>(entity);
		auto& position = renderView.get<Components::Position>(entity);
		auto& scale = renderView.get<Components::Scale>(entity);
			
		if (render.IsEnabled() && position.IsEnabled())
		{
			
			glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity Matrix
			modelMatrix = glm::translate(modelMatrix, position.Get());
			modelMatrix = glm::scale(modelMatrix, scale.Get());

			shader->setMat4("model", modelMatrix);
			render.Draw(*shader);
		}
	}
}

int main()
{
	if (!glfwInit())
	{
		// Initialization failed.
		return -1;
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

	// Doing more GL setup stuff. Comment out some stuff from tutorial to do later.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// glfwSetCursorPosCallback(window, mouse_callback);
	// glfwSetScrollCallback(window, scroll_callback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// Begin ECS
	entt::registry registry;

	const auto camera = registry.create();
	registry.emplace<Components::Camera>(camera, glm::vec3(0.0f, 0.0f, 3.0f));
	const auto model = registry.create();
	//registry.emplace<GameObjectComponent>(glm::vec3(0.0f, 0.0f, 0.0f)); // TODO
	registry.emplace<Components::Renderable>(model, Model("./data/box/cube.obj"));
	registry.emplace<Components::Position>(model, glm::vec3(1.0f, 0.0f, 0.0f));
	registry.emplace<Components::Scale>(model, glm::vec3(1.0f, 1.0f, 1.0f));

	const auto model2 = registry.create();
	registry.emplace<Components::Renderable>(model2, Model("./data/box/cube.obj"));
	registry.emplace<Components::Position>(model2, glm::vec3(-1.0f, 0.0f, 0.0f));
	registry.emplace<Components::Scale>(model2, glm::vec3(1.0f, 1.0f, 1.0f));
	// End ECS

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);

	// Do one-time OpenGL things here.
	Shader* shader = RetrieveShader("model", "./data/shaders/1.model_loading.vs", "./data/shaders/1.model_loading.fs");
	//Model ourModel("./data/box/cube.obj");
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	GameTime::Initialize(glfwGetTime());

	while (!glfwWindowShouldClose(window))
	{
		double currentFrameTime = glfwGetTime();
		double deltaTime = currentFrameTime - GameTime::lastFrameTime;
		GameTime::lastFrameTime = currentFrameTime;
		GameTime::accumulator += deltaTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		while (GameTime::accumulator >= GameTime::fixedDeltaTime)
		{
			// Update game logic for ECS
			update(registry);
			GameTime::accumulator -= GameTime::fixedDeltaTime;
		}
		// Update render objects.
		render(registry);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}