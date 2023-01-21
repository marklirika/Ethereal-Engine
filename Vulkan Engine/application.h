#pragma once

#include "ethereal_device.h" 
#include "ethereal_pipeline.h"
#include "ethereal_swap_chain.h"
#include "ethereal_window.h"

//std
#include <memory>
#include <vector>

namespace ethereal {

	class Application {
		
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		Application();
		~Application();

		Application(const Application&) = delete;
		Application &operator=(const Application&) = delete;

		void run();

	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		EtherealWindow etherealWindow{ WIDTH, HEIGHT, "Ethereal Vulkan Engine" };
		EtherealDevice etherealDevice{ etherealWindow };
		EtherealSwapChain etherealSwapChain{ etherealDevice, etherealWindow.getExtent()};
		std::unique_ptr<EtherealPipeline> etherealPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}