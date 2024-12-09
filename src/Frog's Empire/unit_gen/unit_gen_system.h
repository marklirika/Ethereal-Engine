#pragma once
#include "unit_gen_component.h"
#include "core/ethereal_device.h"

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