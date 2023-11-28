#pragma once
#include "entt.h"

namespace ethereal {

	class Entity;

	class Scene {
	public:
		Scene();
		~Scene();

		Entity createEntity(const std::string& name);
		void removeEntity(Entity entity);  

		entt::registry& getRegistry() { return _registry; }

	private:
		entt::registry _registry;

		friend class Entity;
	};

}
