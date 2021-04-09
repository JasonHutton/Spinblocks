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

		auto followMarkerView = registry.view<Components::Marker, Components::Coordinate, Components::Follower>();
		for (auto entity : followMarkerView)
		{
			auto& coordinate = followMarkerView.get<Components::Coordinate>(entity);
			auto& follower = followMarkerView.get<Components::Follower>(entity);

			if (registry.all_of<Components::Tag>(entity))
			{
				auto& tag = registry.get<Components::Tag>(entity);

				if (tag.Get() == "O Tetromino Marker" || tag.Get() == "I Tetromino Marker")
				{
					
					if (registry.valid(follower.Get()))
					{
						auto& followed = registry.get<Components::Coordinate>(follower.Get());
						coordinate.Set(followed.Get());
						coordinate.SetParent(followed.GetParent());
					}
					else
					{
						registry.destroy(entity);
					}
				}
			}
		}

		/*for (auto leaderEnt : leaderEntities)
		{
			std::vector<Components::Coordinate> coordinates;
			auto& oTetromino = registry.get<Components::OTetromino>(leaderEnt);
			bool bFoundAtLeastOne = false;

			for (int i = 0; i < 4; i++)
			{
				entt::entity blockEnt = oTetromino.GetBlock(i);
				auto& coordinate = registry.get<Components::Coordinate>(blockEnt);
				
				// We're doing this manually instead of using a set.insert because of some possible issues with coordinate's less than comparison. TODO FIXME
				for (Components::Coordinate coord : coordinates)
				{
					if (coord == coordinate)
						bFoundAtLeastOne = true;

					coordinates.push_back(coordinate);
				}
			}

			if (bFoundAtLeastOne)
			{
				int q = 0;
				q++;
			}
		}*/

		/*auto followerView2 = registry.view<Components::Moveable, Components::Coordinate, Components::Follower>();
		for (auto entity : followerView2)
		{
			auto& moveable = followerView2.get<Components::Moveable>(entity);
			auto& coordinate = followerView2.get<Components::Coordinate>(entity);
			auto& follower = followerView2.get<Components::Follower>(entity);

			if (moveable.IsEnabled() && coordinate.IsEnabled() && follower.IsEnabled())
			{
			}
		}*/

		auto followerView3 = registry.view<Components::Moveable, Components::Coordinate, Components::Follower>();
		for (auto entity : followerView3)
		{
			auto& moveable = followerView3.get<Components::Moveable>(entity);
			auto& coordinate = followerView3.get<Components::Coordinate>(entity);
			auto& follower = followerView3.get<Components::Follower>(entity);

			if (moveable.IsEnabled() && coordinate.IsEnabled() && follower.IsEnabled())
			{
				if (!IsEntityTetromino(registry, follower.Get()))
					continue;

				auto* leader = GetTetrominoFromEntity(registry, follower.Get());
				auto& leaderMoveable = registry.get<Components::Moveable>(follower.Get());

				int i;
				for (i = 0; i < 4; i++)
				{
					if (leader->GetBlock(i) == entity)
					{
						break;
					}
				}

				auto offsetCoordinate = Components::Coordinate(leaderMoveable.GetDesiredCoordinate().GetParent(),
					(glm::vec2)leaderMoveable.GetDesiredCoordinate().Get() + leader->GetBlockOffsetCoordinates(leader->GetCurrentOrientation(), i));

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
						if (registry.all_of<Components::Obstructable>(entity))
						{
							auto& obstructable = registry.get<Components::Obstructable>(entity);

							if (registry.all_of<Components::Obstructs>(entity))
							{
								obstructable.SetIsObstructed(true);
							}

							if (IsEntityTetromino(registry, entity))
							{
								auto* tetromino = GetTetrominoFromEntity(registry, entity);
								tetromino->SetAllBlocksObstructed(registry, true);
								tetromino->SetAllBlocksLastObstructedTime(registry, currentFrameTime);
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