#include "Systems/MovementSystem.h"
#include "Utility.h"
#include <set>

namespace Systems
{
	void MovementSystem(entt::registry& registry, double currentFrameTime)
	{
		std::set<entt::entity> leaderEntities;
		auto followerView = registry.view<Components::Moveable, Components::Coordinate, Components::Follower>();
		for (auto entity : followerView)
		{
			auto& moveable = followerView.get<Components::Moveable>(entity);
			auto& coordinate = followerView.get<Components::Coordinate>(entity);
			auto& follower = followerView.get<Components::Follower>(entity);

			leaderEntities.insert(follower.Get());
		}

		auto followerView2 = registry.view<Components::Moveable, Components::Coordinate, Components::Follower>();
		for (auto entity : followerView2)
		{
			auto& moveable = followerView2.get<Components::Moveable>(entity);
			auto& coordinate = followerView2.get<Components::Coordinate>(entity);
			auto& follower = followerView2.get<Components::Follower>(entity);

			if (moveable.IsEnabled() && coordinate.IsEnabled() && follower.IsEnabled())
			{
				auto& leader = registry.get<Components::OTetromino>(follower.Get()); // FIXME TODO tetromino type issue.
				auto& leaderMoveable = registry.get<Components::Moveable>(follower.Get());

				int i;
				for (i = 0; i < 4; i++)
				{
					if (leader.GetBlock(i) == entity)
					{
						break;
					}
				}

				auto offsetCoordinate = Components::Coordinate(leaderMoveable.GetDesiredCoordinate().GetParent(),
					(glm::vec2)leaderMoveable.GetDesiredCoordinate().Get() + leader.GetBlockOffsetCoordinates(i));

				moveable.SetDesiredCoordinate(offsetCoordinate);
				if (moveable.GetCurrentCoordinate() != moveable.GetDesiredCoordinate())
				{
					// Need to detect if a move is allowed before permitting it.
					coordinate = moveable.GetDesiredCoordinate();
					moveable.SetCurrentCoordinate(coordinate);
				}
			}
		}

		auto moveableView = registry.view<Components::Moveable, Components::Coordinate>(entt::exclude<Components::Follower>);
		for (auto entity : moveableView)
		{
			auto& moveable = moveableView.get<Components::Moveable>(entity);
			auto& coordinate = moveableView.get<Components::Coordinate>(entity);

			if (moveable.IsEnabled() && coordinate.IsEnabled())
			{
				switch (moveable.GetMovementState())
				{
				case Components::movementStates_t::DEBUG_MOVE_UP:
				case Components::movementStates_t::SOFT_DROP:
				case Components::movementStates_t::FALL:
				{
					if (moveable.GetCurrentCoordinate() != moveable.GetDesiredCoordinate())
					{
						// Need to detect if a move is allowed before permitting it.
						coordinate = moveable.GetDesiredCoordinate();
						moveable.SetCurrentCoordinate(coordinate);
					}
					break;
				}
				case Components::movementStates_t::HARD_DROP:
				{
					if (moveable.GetCurrentCoordinate() != moveable.GetDesiredCoordinate())
					{
						// Need to detect if a move is allowed before permitting it.
						coordinate = moveable.GetDesiredCoordinate();
						moveable.SetCurrentCoordinate(coordinate);
						if (registry.has<Components::Obstructable>(entity))
						{
							auto& obstructable = registry.get<Components::Obstructable>(entity);
							if (registry.has<Components::Block>(entity))
							{
								obstructable.SetIsObstructed(true);
							}
						}
					}
					break;
				}
				default:
					break;
				}
			}
		}
	}
}