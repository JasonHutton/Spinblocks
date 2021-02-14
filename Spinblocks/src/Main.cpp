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

struct renderModel_t
{
	//std::string model;
	Model model;
	Shader shader;
};

/*struct camera_t
{
	glm::vec4 clearColor; // Color to clear the view to.
	Camera camera; // Camera
	bool enabled; // Is this component enabled? 
	bool setClearColor; // Should the clear color be set this frame?
};*/

class Component
{
public:
	void Enable(bool enable = true);
	const bool& IsEnabled() const;
};

class RenderComponent : public Component
{
private:
	bool m_enabled; // Is the component enabled to systems?

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

class ModelComponent : public RenderComponent
{
private:
	glm::vec3 m_position;
	glm::vec3 m_scale;
public:
	Model m_model;
public:
	ModelComponent(Model model, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)) : m_model(model)
	{
		m_position = position;
		m_scale = scale;
	}
	const glm::vec3& GetPosition() const
	{
		return m_position;
	}
	const glm::vec3& GetScale() const
	{
		return m_scale;
	}
	const Model& GetModel() const
	{
		return m_model;
	}
	void Draw(Shader& shader)
	{
		m_model.Draw(shader);
	}

};

class CameraComponent : public Component
{
private:
	bool m_enabled; // Is the component enabled to systems?
	glm::vec4 m_clearColor; // Color to clear the view to.
	bool m_updateClearColor; // Should the clear color be updated this frame?

public:
	Camera m_camera; // Camera

	CameraComponent(const glm::vec4& clearColor, const glm::vec3& cameraPosition, const bool& updateImmediately = false) : m_camera(cameraPosition)
	{
		Enable(true);
		SetClearColor(clearColor, updateImmediately);
	}

	void SetClearColor(const glm::vec4& clearColor, const bool& updateImmediately = false)
	{
		m_clearColor = clearColor;
		m_updateClearColor = true;

		if (updateImmediately)
			UpdateClearColor();
	}

	const glm::vec4& ClearColor() const
	{
		return m_clearColor;
	}

	const bool& ShouldUpdateClearColor() const
	{
		return m_updateClearColor;
	}

	void UpdateClearColor()
	{
		glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
		m_updateClearColor = false;
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

	Shader* shader = shaders["model"]; // Need to look at this a bit closer.

	glm::mat4 projectionMatrix = glm::mat4(1.0f); // Identity Matrix
	glm::mat4 viewMatrix = glm::mat4(1.0f); // Identity Matrix

	// We're assuming we just have one here.
	auto cameraView = registry.view<CameraComponent>();
	for (auto entity : cameraView)
	{
		auto& camera = cameraView.get<CameraComponent>(entity);
		if (camera.IsEnabled())
		{
			if (camera.ShouldUpdateClearColor())
			{
				camera.UpdateClearColor();
			}

			// view/projection transformations
			projectionMatrix = glm::perspective(glm::radians(camera.m_camera.Zoom), (float)displayData.x / (float)displayData.y, 0.1f, 100.0f);
			viewMatrix = camera.m_camera.GetViewMatrix();
			
			shader->use();
			shader->setMat4("projection", projectionMatrix);
			shader->setMat4("view", viewMatrix);
		}
	}

	auto modelView = registry.view<ModelComponent>();
	for (auto entity : modelView)
	{
		auto& model = modelView.get<ModelComponent>(entity);
		if (model.IsEnabled())
		{
			glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity Matrix
			modelMatrix = glm::translate(modelMatrix, model.GetPosition());
			modelMatrix = glm::scale(modelMatrix, model.GetScale());

			shader->setMat4("model", modelMatrix);
			model.Draw(*shader);
		}
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
	registry.emplace<CameraComponent>(camera, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 3.0f));
	const auto model = registry.create();
	registry.emplace<ModelComponent>(model, Model("./data/box/cube.obj"), glm::vec3(0.0f, 0.0f, 0.0f));
	// End ECS

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);

	// Do one-time OpenGL things here.
	Shader* shader = RetrieveShader("model", "./data/shaders/1.model_loading.vs", "./data/shaders/1.model_loading.fs");
	//Model ourModel("./data/box/cube.obj");
	

	

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
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