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

class Component
{
public:
	void Enable(bool enable = true);
	const bool& IsEnabled() const;
};

class PositionComponent : public Component
{
private:
	bool m_enabled; // Is the component enabled to systems?
	glm::vec3 m_position;
public:
	PositionComponent(const glm::vec3& position)
	{
		m_position = position;
	}
	const glm::vec3& GetPosition() const
	{
		return m_position;
	}
	void SetPosition(const glm::vec3& position)
	{
		m_position = position;
	}
public:
	void Enable(bool enable = true)
	{
		m_enabled = enable;
	}

	const bool& IsEnabled() const
	{
		return m_enabled;
	}
};

class ScaleComponent : public Component
{
private:
	bool m_enabled; // Is the component enabled to systems?
	glm::vec3 m_scale;
public:
	ScaleComponent(const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		m_scale = scale;
	}
	const glm::vec3& GetScale() const
	{
		return m_scale;
	}
	void SetScale(const glm::vec3& scale)
	{
		m_scale = scale;
	}
public:
	void Enable(bool enable = true)
	{
		m_enabled = enable;
	}

	const bool& IsEnabled() const
	{
		return m_enabled;
	}
};

class RenderComponent : public Component
{
private:
	bool m_enabled; // Is the component enabled to systems?

public:
	Model m_model;
public:
	RenderComponent(Model model) : m_model(model)
	{
	}

	const Model& GetModel() const
	{
		return m_model;
	}
	void Draw(Shader& shader)
	{
		m_model.Draw(shader);
	}
public:
	void Enable(bool enable = true)
	{
		m_enabled = enable;
	}

	const bool& IsEnabled() const
	{
		return m_enabled;
	}
};

class CameraComponent : public Component
{
private:
	bool m_enabled; // Is the component enabled to systems?

public:
	Camera m_camera; // Camera

	CameraComponent(const glm::vec3& cameraPosition) : m_camera(cameraPosition)
	{

	}

	void Enable(bool enable = true)
	{
		m_enabled = enable;
	}

	const bool& IsEnabled() const
	{
		return m_enabled;
	}
};

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
	auto cameraView = registry.view<CameraComponent>();
	for (auto entity : cameraView)
	{
		auto& camera = cameraView.get<CameraComponent>(entity);
		if (camera.IsEnabled())
		{
			projectionMatrix = glm::perspective(glm::radians(camera.m_camera.Zoom), (float)displayData.x / (float)displayData.y, 0.1f, 100.0f);
			viewMatrix = camera.m_camera.GetViewMatrix();

			shader->use();
			shader->setMat4("projection", projectionMatrix);
			shader->setMat4("view", viewMatrix);
		}
	}


	auto renderView = registry.view<RenderComponent, PositionComponent, ScaleComponent>();
	for (auto entity : renderView)
	{
		auto& render = renderView.get<RenderComponent>(entity);
		auto& position = renderView.get<PositionComponent>(entity);
		auto& scale = renderView.get<ScaleComponent>(entity);
			
		if (render.IsEnabled() && position.IsEnabled())
		{
			
			glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity Matrix
			modelMatrix = glm::translate(modelMatrix, position.GetPosition());
			modelMatrix = glm::scale(modelMatrix, scale.GetScale());

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
	// glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// glfwSetCursorPosCallback(window, mouse_callback);
	// glfwSetScrollCallback(window, scroll_callback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// Begin ECS
	entt::registry registry;

	const auto camera = registry.create();
	registry.emplace<CameraComponent>(camera, glm::vec3(0.0f, 0.0f, 3.0f));
	const auto model = registry.create();
	//registry.emplace<GameObjectComponent>(glm::vec3(0.0f, 0.0f, 0.0f)); // TODO
	registry.emplace<RenderComponent>(model, Model("./data/box/cube.obj"));
	registry.emplace<PositionComponent>(model, glm::vec3(1.0f, 0.0f, 0.0f));
	registry.emplace<ScaleComponent>(model, glm::vec3(1.0f, 1.0f, 1.0f));

	const auto model2 = registry.create();
	registry.emplace<RenderComponent>(model2, Model("./data/box/cube.obj"));
	registry.emplace<PositionComponent>(model2, glm::vec3(-1.0f, 0.0f, 0.0f));
	registry.emplace<ScaleComponent>(model2, glm::vec3(1.0f, 1.0f, 1.0f));
	// End ECS

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);

	// Do one-time OpenGL things here.
	Shader* shader = RetrieveShader("model", "./data/shaders/1.model_loading.vs", "./data/shaders/1.model_loading.fs");
	//Model ourModel("./data/box/cube.obj");
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Update game logic for ECS
		update(registry);
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