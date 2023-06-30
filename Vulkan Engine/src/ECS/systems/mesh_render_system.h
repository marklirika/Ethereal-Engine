#pragma once
#include "ECS/ethereal_components.h"
#include "core/ethereal_device.h" 
#include "core/ethereal_pipeline.h"
#include "render/ethereal_camera.h"
#include "memory/ethereal_frame_info.h"
#include "render/ethereal_model.h"

//std
#include <memory>
#include <vector>

namespace ethereal {

	class MeshRenderSystem {

	public:
		MeshRenderSystem(EtherealDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~MeshRenderSystem();

		MeshRenderSystem(const MeshRenderSystem&) = delete;
		MeshRenderSystem& operator=(const MeshRenderSystem&) = delete;

		void bind(std::shared_ptr<EtherealModel> model, VkCommandBuffer commandBuffer);
		void draw(std::shared_ptr<EtherealModel> model, VkCommandBuffer commandBuffer);

		void renderGameObjects(FrameInfo &frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);	
		void createPipeline(VkRenderPass renderPass);

		EtherealDevice& etherealDevice;

		std::unique_ptr<EtherealPipeline> etherealPipeline;
		VkPipelineLayout pipelineLayout;
	};
}