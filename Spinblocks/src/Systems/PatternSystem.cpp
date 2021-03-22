#include "Systems/PatternSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

#include <unordered_map>

namespace Systems
{
	void PatternSystem(entt::registry& registry, double currentFrameTime)
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
			for (entt::id_type p = 0; p < linePattern.size(); p++)
			{
				// Check how many blocks are in each row with at least one
				for (glm::uint y = 0; y < linePattern[static_cast<entt::entity>(p)].size(); y++)
				{
					// We've got a full line here!
					size_t sizeOfLine = linePattern[static_cast<entt::entity>(p)][y].size();
					if (sizeOfLine == PlayAreaWidth)
					{
						linesMatched++;
						for (glm::uint x = 0; x < linePattern[static_cast<entt::entity>(p)][y].size(); x++)
						{
							entt::entity blockEntity = linePattern[static_cast<entt::entity>(p)][y][x];
							registry.emplace<Components::Hittable>(blockEntity);
						}
					}
				}
			}
		}
	}
}