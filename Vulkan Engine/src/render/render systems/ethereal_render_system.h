#pragma once

#include "../../core/ethereal_device.h" 
#include "../../core/ethereal_pipeline.h"
#include "../../render/ethereal_camera.h"
#include "../../memory/ethereal_frame_info.h"
#include "../../ECS/ethereal_game_obj.h"

//std
#include <memory>
#include <vector>

namespace ethereal {

	class EtherealRenderSystem {

	public:

		EtherealRenderSystem(EtherealDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~EtherealRenderSystem();

		EtherealRenderSystem(const EtherealRenderSystem&) = delete;
		EtherealRenderSystem& operator=(const EtherealRenderSystem&) = delete;

		void renderGameObjects(FrameInfo &frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);	
		void createPipeline(VkRenderPass renderPass);

		EtherealDevice& etherealDevice;

		std::unique_ptr<EtherealPipeline> etherealPipeline;
		VkPipelineLayout pipelineLayout;
	};
}