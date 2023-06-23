#pragma once

#include "entt.h"

namespace ethereal {
	class Scene	{
		public:
		Scene();
		~Scene();
	private:
		entt::registry m_Registry;

	};
}

