#pragma once

#include "render/ethereal_camera.h"
#include "ECS/ethereal_scene.h"
#include <vulkan/vulkan.h>

namespace ethereal {
	constexpr int MAX_LIGHTS = 10;

	struct PointLight {
		glm::vec4 position{};  // w = 1.0
		glm::vec4 color{}; // w = intensity
	};

	struct GlobalUBO {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };
		PointLight lightPoints[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		EtherealCamera& camera;
		VkDescriptorSet globaDescriptorSet;
		Scene& scene;
	};
}