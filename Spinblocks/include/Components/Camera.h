#pragma once

#include "Component.h"
#include <learnopengl/camera.h>

namespace Components
{
	class Camera : public Component
	{
	private:
		bool m_enabled; // Is the component enabled to systems?

	public:
		// Access the Camera class in the global namespace. We should probably move much of its functionality into this class, but this setup is fine for now. TODO
		::Camera m_camera; // Camera

		Camera(const glm::vec3& cameraPosition) : m_camera(cameraPosition)
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
}