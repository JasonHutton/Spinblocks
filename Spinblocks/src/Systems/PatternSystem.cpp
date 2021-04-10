#include "Systems/PatternSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

#include <unordered_map>

namespace Systems
{
	int PatternSystem(entt::registry& registry, unsigned int lineWidth, double currentFrameTime)
	{
		// linePattern[parentEntity][y][x] = blockEntity (Note y,x ordering, not x,y.)
		auto linePattern = unordered_map<entt::entity, std::unordered_map<glm::uint, std::unordered_map<glm::uint, entt::entity>>>();

		auto blockView = registry.view<Components::Block>();
		for (auto entity : blockView)
		{
			auto& block = blockView.get<Components::Block>(entity);
			auto& moveable = registry.get<Components::Moveable>(entity);
			auto& coordinate = registry.get<Components::Coordinate>(entity);

			if (block.IsEnabled() && moveable.IsEnabled() && coordinate.IsEnabled())
			{
				if (moveable.GetMovementState() == Components::movementStates_t::LOCKED)
				{
					linePattern[coordinate.GetParent()][coordinate.Get().y][coordinate.Get().x] = entity;
				}
			}
		}

		int linesMatched = 0;
		if (linePattern.size() > 0)
		{
			// Ensure we're checking within the same parent container
			for (auto& parent : linePattern)
			{
				// Check how many blocks are in each row with at least one
				for (auto& row : linePattern.at(parent.first))
				{
					// Check if we've got a full line here...
					size_t sizeOfLine = linePattern.at(parent.first).at(row.first).size();
					if (sizeOfLine == lineWidth)
					{
						linesMatched++;

						// We're not actually interested in the column here, we want the entity id.
						for (auto& col : linePattern.at(parent.first).at(row.first))
						{
							const entt::entity& ent = col.second;

							registry.emplace<Components::Hittable>(ent);
						}
					}
				}
			}
		}

		return linesMatched;
	}
}