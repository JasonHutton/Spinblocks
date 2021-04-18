#pragma once

#include "Components/Component.h"
#include <entt/entity/registry.hpp>
#include <vector>

namespace Components
{
	class NodeOrder : public Component
	{
	protected:
		std::vector<entt::entity> m_nodes;
		std::vector<entt::entity>::iterator m_nodeIterator;

	public:
		NodeOrder()
		{
			ResetNodeIterator();
		}

		void AddNode(entt::entity entity)
		{
			m_nodes.push_back(entity);
			ResetNodeIterator();
		}

		entt::entity GetNode()
		{
			if (m_nodeIterator == m_nodes.end())
			{
				ResetNodeIterator();
				return entt::null;
			}

			return *m_nodeIterator++;
		}
	protected:
		void ResetNodeIterator()
		{
			m_nodeIterator = m_nodes.begin();
		}
	};
}