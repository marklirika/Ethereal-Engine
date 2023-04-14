#include "ethereal_render_system.h"

#define GLM_FORCE_RADIANCE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp> 

#include <memory>
#include <vector>
namespace ethereal {

	struct SimplePushConstantData {
		glm::mat2 transform{ 1.f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	EtherealRenderSystem::EtherealRenderSystem(EtherealDevice& device, VkRenderPass renderPass) : etherealDevice{ device } {
		createPipelineLayout();
		createPipeline(renderPass);
	}

	EtherealRenderSystem::~EtherealRenderSystem() {
		vkDestroyPipelineLayout(etherealDevice.device(), pipelineLayout, nullptr);
	}

	void EtherealRenderSystem::createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(etherealDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("fail to create pipelinelayout");
	}

	void EtherealRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pupeline layout");

		PipelineConfigInfo pipelineConfig{};
		EtherealPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		etherealPipeline = std::make_unique<EtherealPipeline>(
			etherealDevice,
			"shaders/vertex_shader.vert.spv",
			"shaders/fragment_shader.frag.spv",
			pipelineConfig);
	}

	void EtherealRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<EtherealGameObject>& gameObjects) {
		etherealPipeline->bind(commandBuffer);

		for (auto& obj : gameObjects) {
			obj.transform2D.rotation = glm::mod(obj.transform2D.rotation + 0.01f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.offset = obj.transform2D.translation;
			push.color = obj.color;
			push.transform = obj.transform2D.mat2();

			vkCmdPushConstants(commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);

			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}
}