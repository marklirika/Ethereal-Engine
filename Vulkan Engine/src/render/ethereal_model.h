#pragma once

#include "../core/ethereal_device.h"
#include "../memory/ethereal_buffer.h"

#define GLM_FORCE_RADIANCE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <vector>
#include <memory>

namespace ethereal {

	class EtherealModel {
	public:
		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions(); 
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		EtherealModel(EtherealDevice& device, const Builder& builder);
		~EtherealModel();

		EtherealModel(const EtherealModel&) = delete;
		EtherealModel& operator=(const EtherealModel&) = delete;

		static std::unique_ptr<EtherealModel> createModelFromFile(EtherealDevice& device, const std::string& filepath);

		// attributes
		std::unique_ptr<EtherealBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<EtherealBuffer> indexBuffer;
		uint32_t indexCount;

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);
		  
		EtherealDevice& etherealDevice;
	};
}