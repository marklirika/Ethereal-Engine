#pragma once
#include "ECS/ethereal_components.h"

namespace ethereal {
	class CollisionSystem {
	public:
		CollisionSystem();
		~CollisionSystem();

		bool sphereCollision();
		bool meshCollison();

		void update();
	};
} // namespace ethereal