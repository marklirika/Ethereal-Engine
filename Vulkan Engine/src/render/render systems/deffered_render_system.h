#pragma once
#include "core/ethereal_device.h" 
#include "memory/ethereal_descriptors.h"
#include "memory/ethereal_frame_info.h"
#include "render/ethereal_pipeline.h"
#include "resources/ethereal_camera.h"
#include "resources/ethereal_model.h"
#include "ECS/ethereal_components.h"

//std
#include <memory>
#include <vector>

namespace ethereal {
	struct DefferedPipeline {
		std::unique_ptr<EtherealPipeline> offscreenPipeline;
		std::unique_ptr<EtherealPipeline> compositionPipeline;
	};

	struct DefferedUniformBuffers {
		std::unique_ptr<EtherealBuffer> offscreen;
		std::unique_ptr<EtherealBuffer> composition;
	};

	class DefferedRenderSystem {
	public:
		DefferedRenderSystem(EtherealDevice& device);
		~DefferedRenderSystem();

		DefferedRenderSystem(const DefferedRenderSystem&) = delete;
		DefferedRenderSystem& operator=(const DefferedRenderSystem&) = delete;

		void bind(std::shared_ptr<EtherealModel> model,	std::shared_ptr<EtherealTexture> texture, FrameInfo& frameInfo);
		void draw(FrameInfo& frameInfo, std::shared_ptr<EtherealModel> model);
		void render(FrameInfo& frameInfo);
	private:
		void writeCmpDescriptors(OffscreenFrameBuffer& offscreenFrmBuffer,
			std::unique_ptr<EtherealBuffer> uboBuffer,
			VkSampler& colorSampler);
		void createSharedPipelineLayout();
		void createPipelines(VkRenderPass compositionRenderPass, VkRenderPass offscreenRenderPass);

		EtherealDevice& etherealDevice;
		VkPipelineLayout pipelineLayout;
		DefferedPipeline defferedPipeline;

		std::unique_ptr<EtherealDescriptorPool> compositionDescriptorPool;
		std::unique_ptr<EtherealDescriptorSetLayout> compositionDescriptorLayout;
		VkDescriptorSet compositionDescriptorSet;
	};
}