#pragma once
#include "entt.h"

namespace ethereal {

	// At first you create scene and then you create entities in it 
	class Entity;

	class Scene	{
		public:
		Scene();
		~Scene();

		Entity createEntity(const std::string& name);

		entt::registry& getRegistry() { return _registry; }
	private:
		entt::registry _registry;
		friend class Entity;
	};
}

