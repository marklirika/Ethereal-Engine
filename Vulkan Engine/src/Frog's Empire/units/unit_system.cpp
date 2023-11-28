#include "unit_system.h"



namespace ethereal {
	void UnitSystem::update(FrameInfo& info) {
		auto view = info.scene.getRegistry().view<UnitComponent>();
		for (auto entity : view) {
			auto& unitCharacteristics = view.get<UnitComponent>(entity);

			if (unitCharacteristics.healhPoint >= 0) {
				info.scene.removeEntity(entity);
			}
			
			
		}

	}

}
