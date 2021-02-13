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

class CameraComponent : Component
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

	//auto view = registry.view<const position, velocity, rgba_t>();
	//auto view = registry.view<camera_t>();
	/*
	* for (auto entity : view)
	{
		auto& camera = view.get<camera_t>(entity);
		if (camera.enabled)
		{
			if (camera.setClearColor)
			{
				glClearColor(camera.clearColor.r, camera.clearColor.g, camera.clearColor.b, camera.clearColor.a);
				camera.setClearColor = false;
			}
		}
	}
	*/
	auto view = registry.view<CameraComponent>();
	for (auto entity : view)
	{
		auto& camera = view.get<CameraComponent>(entity);
		if (camera.IsEnabled())
		{
			if (camera.ShouldUpdateClearColor())
			{
				camera.UpdateClearColor();
			}
		}
	}
	/*view.each([](const auto& camera_t)
	{
		auto& camera = view.get<camera_t>(entity);
		if(camera.enabled)
			glClearColor(camera.clearColor.r, camera.clearColor.g, camera.clearColor.b, camera.clearColor.a);
	});*/

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
	
	/*
	for (auto entity : view)
	{
		auto& rgba = view.get<rgba_t>(entity);
		if(rgba.enabled)
			glClearColor(rgba.rgba.r, rgba.rgba.g, rgba.rgba.b, rgba.rgba.a);
	}*/
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


	// One time initialization things. Generally before we start to render anything.
	// Create an instance of the Importer class
	//Assimp::Importer importer;

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	/*const aiScene* scene = importer.ReadFile("./data/box/cube.obj",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (!scene) {
		//DoTheErrorLogging(importer.GetErrorString());
		return -1;
	}*/

	//scene->
	
	// Begin ECS
	entt::registry registry;

	/* Make 3 entities, each with an rgba_t component. One component is set to be enabled, the others are disabled.
	This causes only one to have any effect, but it triggers in the ECS logic as expected. */
	/*const auto entity = registry.create();
	registry.emplace<rgba_t>(entity, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), false);
	const auto entity2 = registry.create();
	registry.emplace<rgba_t>(entity2, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), false);
	const auto entity3 = registry.create();
	registry.emplace<rgba_t>(entity3, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), false);*/
	const auto camera = registry.create();
	registry.emplace<CameraComponent>(camera, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 3.0f));
	//registry.emplace<camera_t>(camera, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 3.0f), true, true);
	//const auto entity4 = registry.create();
	//registry.emplace<renderModel_t>(entity4);
	// End ECS

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);

	// Do one-time OpenGL things here.
	Shader* shader = RetrieveShader("model", "./data/shaders/1.model_loading.vs", "./data/shaders/1.model_loading.fs");
	Model ourModel("./data/box/cube.obj");

	

	while (!glfwWindowShouldClose(window))
	{
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// Black if glClearColor() is never called.
		//glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Red
		//glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Blue
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Keep running. Generally do stuff
		update(registry);


		/*
		// don't forget to enable shader before setting uniforms
		ourShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)displayData.x / (float)displayData.y, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);*/



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