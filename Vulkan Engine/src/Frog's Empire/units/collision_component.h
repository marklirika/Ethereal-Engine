#pragma once
#include "memory/ethereal_frame_info.h"
#include "ECS/ethereal_entity.h"
#include "ECS/ethereal_scene.h"
#include "ECS/ethereal_components.h"


struct CollisionComponent {
	float radius;
	float height;
	bool ghostMode;
};