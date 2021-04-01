#pragma once

#include "Components/Component.h"
#include <learnopengl/camera.h>

namespace Components
{
	class Camera : public Component
	{
	protected:
		// Access the Camera class in the global namespace. We should probably move much of its functionality into this class, but this setup is fine for now. TODO
		::Camera m_camera; // Camera

		glm::mat4 m_projectionMatrix; // Initialize to Identity Matrix
		//glm::mat4 m_viewMatrix; // Initialize to Identity Matrix

	public:
		Camera(const glm::vec3& cameraPosition) : m_camera(cameraPosition), m_projectionMatrix(glm::mat4(1.0f))//, m_viewMatrix(glm::mat4(1.0f))
		{
		}

		const glm::mat4& GetProjectionMatrix() const
		{
			return m_projectionMatrix;
		}

		void SetProjectionMatrix(const glm::mat4& projectionMatrix)
		{
			m_projectionMatrix = projectionMatrix;
		}

		//virtual void UpdateProjectionMatrix(float left, float right, float bottom, float top, float zNear, float zFar) = 0;

		// Pulls from the internal Camera member. Rewrite this later probably. TODO
		glm::mat4 GetViewMatrix()
		{
			return m_camera.GetViewMatrix();
		}
	};

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(const glm::vec3& cameraPosition) : Camera(cameraPosition)
		{
		}

		void UpdateProjectionMatrix()
		{
			m_projectionMatrix = glm::perspective(glm::radians(m_camera.Zoom), (float)displayData.x / (float)displayData.y, 0.1f, 100.0f);
		}
	};

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(const glm::vec3& cameraPosition) : Camera(cameraPosition)
		{
		}

		void UpdateProjectionMatrix()
		{
			m_projectionMatrix = glm::ortho(0.0f, (float)displayData.x, 0.0f, (float)displayData.y, 0.1f, 100.0f); // Lower-left origin, y+ is up
			//m_projectionMatrix = glm::ortho(0.0f, (float)displayData.x, (float)displayData.y, 0.0f, 0.1f, 100.0f); // Upper-left origin, y+ is down
		}
	};
}