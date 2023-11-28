#include "unit_gen_system.h"
#include <glm/gtx/quaternion.hpp> // Include this header to access glm::angleAxis
#include <glm/gtx/euler_angles.hpp>
#include <iostream>

namespace ethereal {
	
	UnitGenSystem::UnitGenSystem(EtherealDevice& device) : etherealDevice(device) { 
		offset = 0;
		etherealModel = EtherealModel::createModelFromFile(etherealDevice, "models/16433_Pig.obj");
	}

	void UnitGenSystem::generate(FrameInfo& info) {
		auto view = info.scene.getRegistry().view<UnitGenComponent>();
		for (auto entity : view) {
			auto& unitGen = view.get<UnitGenComponent>(entity);
			
			if (unitGen.queue ) {
				unitGen.queue = (unitGen.queue > unitGen.limit) ? unitGen.limit : unitGen.queue;
				unitGen.processTime += info.frameTime;
				if (unitGen.processTime <= -unitGen.finishTime) {
					Entity newUnit = info.scene.createEntity("unit");
					newUnit.addComponent<MeshComponent>(etherealModel);
					auto& newUnitTransform = newUnit.getComponent<TransformComponent>();
					newUnitTransform.scale = { 0.3f, 0.3f, 0.3f };
					newUnitTransform.translation = {unitGen.spawnPoint.x + offset, unitGen.spawnPoint.y, unitGen.spawnPoint.z};
					
					//movement and rotation towards destination
					auto& newUnitMovement = newUnit.addComponent<MovementComponent>();
					newUnitMovement.destination = unitGen.destinationPoint;
					//newUnitTransform.rotation = {glm::radians(90.f), glm::radians(90.f), glm::radians(90.f)};
					newUnitTransform.scale = {2.f, 2.f, 2.f};
					// ¬ычисл€ем векторы движени€ и текущего направлени€ модели
					glm::vec3 direction = glm::normalize(newUnitMovement.destination - newUnitTransform.translation);
					glm::vec3 forwardVector(0.0f, 0.0f, -1.0f); // ѕредполагаем, что изначально моделька смотрит вдоль оси Z
					// ¬ычисл€ем скал€рное произведение двух нормализованных векторов
					float dotProduct = glm::dot(direction, forwardVector);
					// ¬ычисл€ем угол между векторами в радианах
					float angleRad = acos(dotProduct);
					newUnitTransform.rotation = { glm::radians(90.f), angleRad, 0.f };
					// —оздаем матрицу поворота вокруг оси Y на вычисленный угол
					//glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angleRad, glm::vec3(0.0f, 1.0f, 0.0f));

					//unitComponent
					auto& newUnitCharacteristics =  newUnit.addComponent<UnitComponent>();
					newUnitCharacteristics.healhPoint = 100;
					if (newUnitCharacteristics.healhPoint >= 0){newUnit.re}
					//temporary
					newUnitCharacteristics.damage = 10;
		


					newUnitMovement.speed = 1; 
					newUnitMovement.isMoving = true;
					

					//offset -= 3;
					unitGen.queue -= 1; 
					unitGen.processTime = 0;
					std::cout << newUnitTransform.translation.x << "\t" << newUnitTransform.translation.y << "\t" << newUnitTransform.translation.z << "\n";
					std::cout << "unit generated, destenation " << newUnitMovement.destination.x << "\t" << newUnitMovement.destination.y << "\t" << newUnitMovement.destination.z << "\t" << "\n";
					
				}
			}
		}
	}
} // namespace ethereal