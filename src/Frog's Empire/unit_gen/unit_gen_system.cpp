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
			if (unitGen.isActive) {
				unitGen.processTime += info.frameTime;
			}
			if (unitGen.processTime <= -unitGen.finishTime) {
				unitGen.processTime = 0;
				Entity newUnit = info.scene.createEntity("unit");
				newUnit.addComponent<MeshComponent>(etherealModel);

				newUnit.getComponent<TransformComponent>().translation = { 0.f + offset , 0.f, 0.f };
				offset += 10;
			}
		}
	}
} // namespace ethereal