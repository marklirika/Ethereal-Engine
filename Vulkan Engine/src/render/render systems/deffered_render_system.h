#pragma once
#include "core/ethereal_device.h" 
#include "render/ethereal_pipeline.h"
#include "memory/ethereal_descriptors.h"
#include "memory/ethereal_frame_info.h"
#include "resources/ethereal_camera.h"
#include "resources/ethereal_model.h"
#include "ECS/ethereal_components.h"
#include "render/ethereal_renderer.h"
//std
#include <memory>
#include <vector>

namespace ethereal {
	struct DefferedPipeline {
		std::unique_ptr<EtherealPipeline> offscreenPipeline;
		std::unique_ptr<EtherealPipeline> compositionPipeline;
	};

	class DefferedRenderSystem {
	public:
		DefferedRenderSystem(EtherealDevice& device,
			OffscreenFrameBuffer& offscreenFrmBuffer,
			VkRenderPass compositionRenderPass,
			VkDescriptorSetLayout globalSetLayout);
		~DefferedRenderSystem() {}

		DefferedRenderSystem(const DefferedRenderSystem&) = delete;
		DefferedRenderSystem& operator=(const DefferedRenderSystem&) = delete;

		void bind(FrameInfo& frameInfo,	std::shared_ptr<EtherealModel> model, std::shared_ptr<EtherealTexture> texture);
		void draw(FrameInfo& frameInfo, std::shared_ptr<EtherealModel> model);
		void render(FrameInfo& frameInfo);
	private:
		void createColorSampler();
		void writeCmpDescriptors(OffscreenFrameBuffer& offscreenFrmBuffer);
		void createSharedPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipelines(VkRenderPass offscreenRenderPass, VkRenderPass compositionRenderPass);

		std::unique_ptr<EtherealDescriptorPool> compositionDescriptorPool;
		std::unique_ptr<EtherealDescriptorSetLayout> compositionDescriptorLayout;
		std::vector<VkDescriptorSet> compositionDescriptorSets { EtherealSwapChain::MAX_FRAMES_IN_FLIGHT };

		VkSampler colorSampler;
		DefferedPipeline defferedPipelines;
		VkPipelineLayout pipelineLayout;
		EtherealDevice& etherealDevice;
	};
}