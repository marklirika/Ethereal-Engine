#include "unit_gen_system.h"
#include "iostream"

namespace ethereal {
	
	UnitGenSystem::UnitGenSystem(EtherealDevice& device) : etherealDevice(device) { 
		offset = 0;
		etherealModel = EtherealModel::createModelFromFile(etherealDevice, "models/frog_1.obj");

	}

	void UnitGenSystem::generate(FrameInfo& info) {
		auto view = info.scene.getRegistry().view<UnitGenComponent>();
		for (auto entity : view) {
			auto& unitGen = view.get<UnitGenComponent>(entity);
			
			if (unitGen.queue ) {
				unitGen.queue = (unitGen.queue > unitGen.limit) ? unitGen.limit : unitGen.queue;
				unitGen.processTime += info.frameTime;
				if (unitGen.processTime <= -unitGen.finishTime) {
					unitGen.processTime = 0;
					
					/// frogs
				
					Entity newUnit = info.scene.createEntity("unit");
					newUnit.addComponent<MeshComponent>(etherealModel);
					auto& newUnitTransform = newUnit.getComponent<TransformComponent>();
					newUnitTransform.scale = { 0.3f, 0.3f, 0.3f };
					newUnitTransform.translation = {unitGen.spawnPoint.x + offset, unitGen.spawnPoint.y, unitGen.spawnPoint.z};
					newUnitTransform.rotation += glm::radians(90.0f);
					offset -= 3;
					unitGen.queue -= 1; 
					
				}
			}
		}
	}
} // namespace ethereal