#pragma once

#include "Components/Component.h"
#include <learnopengl/model.h>

namespace Components
{
	// This gets iterated over. Ensure values are continual.
	enum renderLayer_t
	{
		RL_MIN = 0,
		RL_CONTAINER,
		RL_CELL,
		RL_MARKER_UNDER,
		RL_TETROMINO,
		RL_BLOCK,
		RL_MARKER_OVER,
		RL_MAX
	};

	class Renderable : public Component
	{
	public:
		Model m_model;
		renderLayer_t m_renderLayer;

	public:
		Renderable(renderLayer_t renderLayer, Model model, bool enabled = true) : m_model(model), m_renderLayer(renderLayer), Component(enabled)
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