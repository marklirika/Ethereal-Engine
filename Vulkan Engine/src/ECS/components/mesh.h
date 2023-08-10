#pragma once
#include "resources/ethereal_model.h"
#include <memory>

namespace ethereal {

	struct MeshComponent {
		std::shared_ptr<EtherealModel> model;
	};

} // namespace ethereal