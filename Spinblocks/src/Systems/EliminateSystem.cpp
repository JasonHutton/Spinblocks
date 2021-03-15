#include "Systems/EliminateSystem.h"
#include "Systems/SystemShared.h"
#include "Utility.h"

namespace Systems
{
	void EliminateSystem(entt::registry& registry, double currentFrameTime)
	{
		// Clear all hitlist marked ents
		auto hittableView = registry.view<Components::Block, Components::Hittable>();
		for (auto entity : hittableView)
		{
			registry.destroy(entity);
		}
	}
}