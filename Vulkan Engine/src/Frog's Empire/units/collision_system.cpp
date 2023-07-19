#include "collision_system.h"


namespace ethereal {
	void CollisionSystem::checkCollision(FrameInfo& info) {
		auto view = info.scene.getRegistry().view<TransformComponent>();
		for (auto entity : view) {

		}

	}
} // ethereal