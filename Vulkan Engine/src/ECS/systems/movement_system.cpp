#include "movement_system.h"


namespace ethereal {
	void MovementSystem::move(FrameInfo& info) {
		auto view = info.scene.getRegistry().view<MovementComponent, TransformComponent>();
		for (auto entity : view) {
			auto& unitMovement = view.get<MovementComponent>(entity);
			unitMovement.isMoving = true;
			auto& unitTranform = view.get<TransformComponent>(entity);
			unitTranform.translation.x = (unitTranform.translation.x != unitMovement.destination.x) ? unitTranform.translation.x + (info.frameTime * unitMovement.speed) : unitTranform.translation.x;
			unitTranform.translation.y = (unitTranform.translation.y != unitMovement.destination.y) ? unitTranform.translation.y + (info.frameTime * unitMovement.speed) : unitTranform.translation.y;
			unitTranform.translation.z = (unitTranform.translation.z != unitMovement.destination.z) ? unitTranform.translation.z + (info.frameTime * unitMovement.speed) : unitTranform.translation.z;
			unitMovement.isMoving = false;
		}

	}
} // ethereal
