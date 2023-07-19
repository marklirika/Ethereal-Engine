#pragma once
#include "render/ethereal_model.h"
#include <memory>

namespace ethereal {

	struct MeshComponent {
		std::shared_ptr<EtherealModel> model;
	};

} // namespace ethereal