#pragma once
#include "ECS/ethereal_components.h"
#include "core/ethereal_device.h"
#include "ECS/components/movement_component.h"

namespace ethereal {

	class UnitGenSystem {
	public:
		UnitGenSystem(EtherealDevice& device);
		UnitGenSystem(const UnitGenSystem&) = delete;
		UnitGenSystem& operator=(const UnitGenSystem&) = delete;

		void generate(FrameInfo& info);
	
	private:
		std::shared_ptr<EtherealModel> etherealModel;
		EtherealDevice& etherealDevice;
		int offset;
	};

} // namespace ethereal