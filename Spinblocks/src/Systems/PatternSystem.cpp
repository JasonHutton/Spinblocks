#include "Systems/PatternSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

#include <unordered_map>

namespace Systems
{
	int PatternSystem(entt::registry& registry, unsigned int lineWidth, double currentFrameTime)
	{
		// linePattern[parentEntity][y][x] = blockEntity (Note y,x ordering, not x,y.)
		// East-West. So y,x, not x,y. Horizontal
		auto linePatternEW = unordered_map<entt::entity, std::unordered_map<glm::uint, std::unordered_map<glm::uint, entt::entity>>>();
		// North-South. So, x,y, not y,x. Vertical
		auto linePatternNS = unordered_map<entt::entity, std::unordered_map<glm::uint, std::unordered_map<glm::uint, entt::entity>>>();

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
					linePatternEW[coordinate.GetParent()][coordinate.Get().y][coordinate.Get().x] = entity;
					linePatternNS[coordinate.GetParent()][coordinate.Get().x][coordinate.Get().y] = entity;
				}
			}
		}

		const auto& playAreaEnt = FindEntityByTag(registry, GetTagFromContainerType(containerType_t::PLAY_AREA));
		if (playAreaEnt == entt::null)
			throw std::runtime_error("Play Area entity is null!");
		auto& playAreaDirection = registry.get<Components::CardinalDirection>(playAreaEnt);

		int linesMatched = 0;
		if (playAreaDirection.GetCurrentOrientation() == moveDirection_t::NORTH || playAreaDirection.GetCurrentOrientation() == moveDirection_t::SOUTH)
		{ // North/South orientation. Deal with East/West lines.
			if (linePatternEW.size() > 0)
			{
				// Ensure we're checking within the same parent container
				for (auto& parent : linePatternEW)
				{
					// Check how many blocks are in each row with at least one
					for (auto& row : linePatternEW.at(parent.first))
					{
						// Check if we've got a full line here...
						size_t sizeOfLine = linePatternEW.at(parent.first).at(row.first).size();
						if (sizeOfLine == lineWidth)
						{
							linesMatched++;

							// We're not actually interested in the column here, we want the entity id.
							for (auto& col : linePatternEW.at(parent.first).at(row.first))
							{
								const entt::entity& ent = col.second;

								registry.emplace<Components::Hittable>(ent);
							}
						}
					}
				}
			}
		}
		else // East || West
		{ // East/West orientation. Deal with North/South lines.
			if (linePatternNS.size() > 0)
			{
				// Ensure we're checking within the same parent container
				for (auto& parent : linePatternNS)
				{
					// Check how many blocks are in each column with at least one
					for (auto& col : linePatternNS.at(parent.first))
					{
						// Check if we've got a full line here...
						size_t sizeOfLine = linePatternNS.at(parent.first).at(col.first).size();
						if (sizeOfLine == lineWidth)
						{
							linesMatched++;

							// We're not actually interested in the row here, we want the entity id.
							for (auto& row : linePatternEW.at(parent.first).at(col.first))
							{
								const entt::entity& ent = row.second;

								registry.emplace<Components::Hittable>(ent);
							}
						}
					}
				}
			}
		}

		return linesMatched;
	}
}