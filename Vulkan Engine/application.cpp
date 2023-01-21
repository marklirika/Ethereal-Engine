#include "application.h"

//std
#include <stdexcept>;

namespace ethereal {

	Application::Application() {
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	Application::~Application() {
		vkDestroyPipelineLayout(etherealDevice.device(), pipelineLayout, nullptr); 
	}

	void Application::run() {
		while (!etherealWindow.shouldClose()){
			glfwPollEvents();
		}
	}

	void Application::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(etherealDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("fail to create pipelinelayout");
	}

	void Application::createPipeline() {
		PipelineConfigInfo pipelineConfig{};
		EtherealPipeline::defaultPipelineConfigInfo(pipelineConfig, etherealSwapChain.width(), etherealSwapChain.height());
		pipelineConfig.renderPass = etherealSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
 		etherealPipeline = std::make_unique<EtherealPipeline>(
			etherealDevice,
			"shaders/vertex_shader.vert.spv",
			"shaders/fragment_shader.frag.spv",
			pipelineConfig);
	}

	void Application::createCommandBuffers() {
		commandBuffers.resize(etherealSwapChain.imageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	}
	void Application::drawFrame() {}
}