#pragma once

#include "resources/ethereal_camera.h"
#include "ECS/ethereal_scene.h"
#include <vulkan/vulkan.h>

namespace ethereal {
	constexpr int MAX_LIGHTS = 50;

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
	
	struct FrameBufferAttachment {
		VkImage image;
		VkDeviceMemory mem;
		VkImageView view;
		VkFormat format;
	};

	//Deffered Shading part
	//************************************************

	struct OffscreenUBO {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };
	};

	struct CompositionUBO {
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };
		PointLight lightPoints[MAX_LIGHTS];
		int numLights;
	};

	//************************************************
	//Defferd Shading part
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer offscreenCmdBuffer;
		VkCommandBuffer commandBuffer;
		VkDescriptorSet globaDescriptorSet;
		EtherealCamera& camera;
		Scene& scene;
	};
}