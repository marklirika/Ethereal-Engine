#pragma once

#include "ethereal_device.h"

#define GLM_FORCE_RADIANCE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <vector>

namespace ethereal {

	class EtherealModel {
	public:
		struct Vertex {
			glm::vec2 position;
			glm::vec3 color;
			
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions(); 
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		};

		EtherealModel(EtherealDevice& device, const std::vector<Vertex>& vertices);
		~EtherealModel();

		EtherealModel(const EtherealModel&) = delete;
		EtherealModel& operator=(const EtherealModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffer(const std::vector<Vertex>& vertices);

		EtherealDevice& etherealDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}