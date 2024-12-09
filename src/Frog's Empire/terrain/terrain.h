#pragma once
#include "utility/open_simplex_noise.h"
#include "ECS/ethereal_entity.h"
#include "ECS/ethereal_components.h"

//glm
#include <glm/glm.hpp>

namespace Frogs_Empire {
	/*
	*/

	class  Terrain {
	public:
		std::vector<ethereal::Entity> generateObjects();		
		static std::unique_ptr<ethereal::EtherealModel> generateTerrain(ethereal::EtherealDevice& device, glm::vec2 map_size, glm::vec2 unit_size);

	private:
		//overall value of map, higher value - more expencive objects and more objects overall will be generated
		int value;
		//size of map, for exampe 256x256, 512x512, 1024x1024 units
		glm::vec2 unit_size;
		glm::vec2 map_size;
	};

} // namespace ethereal