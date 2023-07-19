#pragma once
#include "memory/ethereal_frame_info.h"
#include "ECS/ethereal_entity.h"
#include "ECS/ethereal_scene.h"
#include "ECS/ethereal_components.h"

struct MovementComponent {
	bool isMoving;
	float speed; 
	glm::vec3 destination {};
};

