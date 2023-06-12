#pragma once

#include "ethereal_device.h" 
#include "ethereal_pipeline.h"
#include "ethereal_camera.h"
#include "ethereal_frame_info.h"
#include "ethereal_game_obj.h"


//std
#include <memory>
#include <vector>

namespace ethereal {

	class LightPointRenderSystem {

	public:

		LightPointRenderSystem(EtherealDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~LightPointRenderSystem();

		LightPointRenderSystem(const LightPointRenderSystem&) = delete;
		LightPointRenderSystem& operator=(const LightPointRenderSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUBO& ubo);
		void render(FrameInfo &frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);	
		void createPipeline(VkRenderPass renderPass);

		EtherealDevice& etherealDevice;

		std::unique_ptr<EtherealPipeline> etherealPipeline;
		VkPipelineLayout pipelineLayout;
	};
}