#include "mesh_render_system.h"

//GLFW
#define GLM_FORCE_RADIANCE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp> 

//std
#include <memory>
#include <vector>

namespace ethereal {

	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	MeshRenderSystem::MeshRenderSystem(EtherealDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) 
		: etherealDevice{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	MeshRenderSystem::~MeshRenderSystem() {
		vkDestroyPipelineLayout(etherealDevice.device(), pipelineLayout, nullptr);
	}

	void MeshRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts { globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(etherealDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("fail to create pipelinelayout");
	}

	void MeshRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pupeline layout");

		PipelineConfigInfo pipelineConfig{};
		EtherealPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		etherealPipeline = std::make_unique<EtherealPipeline>(
			etherealDevice,
			"shaders/mesh.vert.spv",
			"shaders/mesh.frag.spv",
			pipelineConfig);
	}

	void MeshRenderSystem::bind(std::shared_ptr<EtherealModel> model, VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { model->vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		if (model->hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, model->indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void MeshRenderSystem::draw(std::shared_ptr<EtherealModel> model, VkCommandBuffer commandBuffer) {
		if (model->hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, model->indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, model->vertexCount, 1, 0, 0);
		}
	}

	void MeshRenderSystem::renderGameObjects(FrameInfo& frameInfo) {
		etherealPipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globaDescriptorSet,
			0,
			nullptr);

		auto view = frameInfo.scene.getRegistry().view<MeshComponent,TransformComponent>();
		for (auto entity : view) {
			auto& mesh = view.get<MeshComponent>(entity);
			auto& transform = view.get<TransformComponent>(entity);
			SimplePushConstantData push{};
			push.modelMatrix = transform.mat4();
			push.normalMatrix = transform.normalMatrix();

			vkCmdPushConstants(frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);

			bind(mesh.model, frameInfo.commandBuffer);
			draw(mesh.model, frameInfo.commandBuffer);
		}
	}
}