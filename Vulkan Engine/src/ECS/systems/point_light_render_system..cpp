#include "point_light_render_system.h"

//glm
#define GLM_FORCE_RADIANCE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp> 

//std
#include <memory>
#include <vector>
#include <map>

namespace ethereal {

	struct PointLightPushConstants {
		glm::vec4 position{};
		glm::vec4 color{};
		float radius;
	};

	PointLightRenderSystem::PointLightRenderSystem(EtherealDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
		: etherealDevice{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	PointLightRenderSystem::~PointLightRenderSystem() {
		vkDestroyPipelineLayout(etherealDevice.device(), pipelineLayout, nullptr);
	}

	void PointLightRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstants);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(etherealDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("fail to create pipelinelayout");
	}

	void PointLightRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pupeline layout");

		PipelineConfigInfo pipelineConfig{};
		EtherealPipeline::defaultPipelineConfigInfo(pipelineConfig);
		EtherealPipeline::enableAlphaBlending(pipelineConfig);

		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.attributeDescriptions.clear();

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		etherealPipeline = std::make_unique<EtherealPipeline>(
			etherealDevice,
			"shaders/point_light.vert.spv",
			"shaders/point_light.frag.spv",
			pipelineConfig);
	}

	void PointLightRenderSystem::update(FrameInfo& frameInfo, GlobalUBO& ubo) {

		auto rotateLight = glm::rotate(
			glm::mat4{ 1.f }, frameInfo.frameTime, { 0.f, -1.f, 0.f });

		int lightIndex = 0;
		auto view = frameInfo.scene.getRegistry().view<TransformComponent, PointLightComponent>();
		for (auto& entity : view) {
			auto& transform = view.get<TransformComponent>(entity);
			auto& pointLight = view.get<PointLightComponent>(entity);

			assert(lightIndex < MAX_LIGHTS && "LIGHTS AMOUNT > MAX_LIGHTS");
			// copy point to ubo
			ubo.lightPoints[lightIndex].position = glm::vec4(transform.translation, 1.f);
			ubo.lightPoints[lightIndex].color = glm::vec4(pointLight.color, pointLight.lightIntensity);

			lightIndex += 1;
		}
		ubo.numLights = lightIndex;
	}

	void PointLightRenderSystem::render(FrameInfo& frameInfo) {
		auto view = frameInfo.scene.getRegistry().view<TransformComponent, PointLightComponent>();
		etherealPipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globaDescriptorSet,
			0,
			nullptr);
		
		for (auto& entity : view) {
			auto& transform = view.get<TransformComponent>(entity);
			auto& pointLight = view.get<PointLightComponent>(entity);
			PointLightPushConstants push{};
			push.position = glm::vec4(transform.translation, 1.f);
			push.color = glm::vec4(pointLight.color, pointLight.lightIntensity);
			push.radius = transform.scale.x / 100;

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PointLightPushConstants),
				&push
			);
			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0); 
		}
	}
}