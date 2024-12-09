#pragma once

#include "core/ethereal_device.h" 
#include "core/ethereal_pipeline.h"
#include "render/ethereal_camera.h"
#include "memory/ethereal_frame_info.h"
#include "ECS/ethereal_components.h"

//std
#include <memory>
#include <vector>

namespace ethereal {

	class PointLightRenderSystem {

	public:

		PointLightRenderSystem(EtherealDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightRenderSystem();

		PointLightRenderSystem(const PointLightRenderSystem&) = delete;
		PointLightRenderSystem& operator=(const PointLightRenderSystem&) = delete;

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