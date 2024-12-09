#pragma once
#include <chrono>
#include "memory/ethereal_frame_info.h"
#include "ECS/ethereal_entity.h"
#include "ECS/ethereal_scene.h"
#include "ECS/ethereal_components.h"

struct UnitGenComponent {
	float finishTime = std::chrono::seconds(10).count();
	float processTime;
	bool isActive;
};