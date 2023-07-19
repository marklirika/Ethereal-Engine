#pragma once
#include "render/ethereal_model.h"

namespace ethereal {

	struct CollisionComponent {

        glm::vec3 calculateMeshCenter(const std::vector<EtherealModel::Vertex>& vertices);

		float calculateMeshRadius(const std::vector<EtherealModel::Vertex>& vertices);

		glm::vec3 center{0.f};
		float radius = 0.f;
		bool collision = false;

	};

} // namespace ethereal