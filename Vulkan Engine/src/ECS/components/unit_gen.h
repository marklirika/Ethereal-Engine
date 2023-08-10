#pragma once
#include <chrono>
#include "memory/ethereal_frame_info.h"
#include "ECS/ethereal_entity.h"
#include "ECS/ethereal_scene.h"
#include "ECS/ethereal_components.h"
#include <glm/glm.hpp>


struct UnitGenComponent {
	float finishTime = std::chrono::seconds(5).count();
	float processTime;
	
	//queue
	int queue;
	int limit;

	//spawn position
	glm::vec3 spawnPoint {};
	
	//destination position
	glm::vec3 destinationPoint {};
};