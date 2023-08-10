#include "movement_system.h"
#include <iostream>



namespace ethereal {
	void MovementSystem::move(FrameInfo& info) {
		auto view = info.scene.getRegistry().view<MovementComponent, TransformComponent>();
		for (auto entity : view) {
			auto& unitMovement = view.get<MovementComponent>(entity);
			
			if (unitMovement.isMoving == true) {
				auto& unitTransform = view.get<TransformComponent>(entity);
				glm::vec3 targetPosition = unitMovement.destination;
				glm::vec3 currentPosition = unitTransform.translation;
				glm::vec3 direction = targetPosition - currentPosition;
				float distanceToTarget = glm::length(direction);

				if (distanceToTarget > 0.01f) { 
					float distanceToMove = unitMovement.speed * info.frameTime;
					glm::vec3 movement = glm::normalize(direction) * std::min(distanceToMove, distanceToTarget);
					unitTransform.translation -= movement;

					//std::cout << unitTransform.translation.x << "\t" << unitTransform.translation.y << "\t" << unitTransform.translation.z << "\n";
				}
				else {
					std::cout << "unit reached the destination " << unitTransform.translation.x << "\t" << unitTransform.translation.y << "\t" << unitTransform.translation.z << "\n";
					unitTransform.translation = targetPosition;
					unitMovement.isMoving = false;
				}
				}
			}	
		          
		}

	}
