#pragma once

#include "Component.h"
#include <learnopengl/model.h>

namespace Components
{
	class Renderable : public Component
	{
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
	};
}