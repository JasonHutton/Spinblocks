#pragma once

#include "Component.h"
#include <learnopengl/model.h>

namespace Components
{
	class Renderable : public Component
	{
	private:
		bool m_enabled; // Is the component enabled to systems?

	public:
		Model m_model;
	public:
		Renderable(Model model) : m_model(model)
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
}