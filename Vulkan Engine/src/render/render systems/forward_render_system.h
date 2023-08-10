#pragma once
#include "ECS/ethereal_components.h"
#include "core/ethereal_device.h" 
#include "render/ethereal_pipeline.h"
#include "memory/ethereal_descriptors.h"
#include "memory/ethereal_frame_info.h"
#include "resources/ethereal_camera.h"
#include "resources/ethereal_model.h"

//std
#include <memory>
#include <vector>

namespace ethereal {

	class ForwardRenderSystem {
	public:
		ForwardRenderSystem(EtherealDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~ForwardRenderSystem();

		ForwardRenderSystem(const ForwardRenderSystem&) = delete;
		ForwardRenderSystem& operator=(const ForwardRenderSystem&) = delete;

		void bind(std::shared_ptr<EtherealModel> model, 
			std::shared_ptr<EtherealTexture> texture, 
			FrameInfo& frameInfo);
		void draw(std::shared_ptr<EtherealModel> model, VkCommandBuffer commandBuffer);

		void render(FrameInfo &frameInfo);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);	
		void createPipeline(VkRenderPass renderPass);

		std::unique_ptr<EtherealPipeline> etherealPipeline;
		VkPipelineLayout pipelineLayout;
		EtherealDevice& etherealDevice;
	};

}