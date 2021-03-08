#pragma once

#include "Component.h"
#include <learnopengl/model.h>

namespace Components
{
	// This gets iterated over. Ensure values are continual.
	enum renderLayer_t
	{
		RL_MIN = 0,
		RL_CONTAINER,
		RL_CELL,
		RL_BLOCK,
		RL_MAX
	};

	class Renderable : public Component
	{
	public:
		Model m_model;
		renderLayer_t m_renderLayer;

	public:
		Renderable(renderLayer_t renderLayer, Model model) : m_model(model), m_renderLayer(renderLayer)
		{
		}

		const Model& GetModel() const
		{
			return m_model;
		}

		const renderLayer_t& GetLayer() const
		{
			return m_renderLayer;
		}

		void Draw(Shader& shader)
		{
			m_model.Draw(shader);
		}
	};
}