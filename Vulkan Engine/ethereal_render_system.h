#pragma once
#pragma once

#include "ethereal_device.h" 
#include "ethereal_pipeline.h"
#include "ethereal_game_obj.h"

//std
#include <memory>
#include <vector>

namespace ethereal {

	class EtherealRenderSystem {

	public:

		EtherealRenderSystem(EtherealDevice& device, VkRenderPass renderPass);
		~EtherealRenderSystem();

		EtherealRenderSystem(const EtherealRenderSystem&) = delete;
		EtherealRenderSystem& operator=(const EtherealRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<EtherealGameObject>& gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		EtherealDevice& etherealDevice;

		std::unique_ptr<EtherealPipeline> etherealPipeline;
		VkPipelineLayout pipelineLayout;
	};
}