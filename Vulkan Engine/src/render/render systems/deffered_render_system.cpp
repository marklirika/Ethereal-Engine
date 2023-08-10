#include "deffered_render_system.h"

namespace ethereal {

	void DefferedRenderSystem::createSharedPipelineLayout() {

		std::vector<VkDescriptorSetLayout> Layout = { compositionDescriptorLayout->getDescriptorSetLayout() };
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(Layout.size());
		pipelineLayoutInfo.pSetLayouts = Layout.data();

		if (vkCreatePipelineLayout(etherealDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout)) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void DefferedRenderSystem::createPipelines(VkRenderPass compositionRenderPass, VkRenderPass offscreenRenderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pupeline layout");

		PipelineConfigInfo pipelineConfig{};
		EtherealPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.pipelineLayout = pipelineLayout;

		//Offscreen pipeline
		pipelineConfig.renderPass = offscreenRenderPass;
		pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
		pipelineColorBlendAttachmentState.colorWriteMask = 0xf;
		pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
		std::array<VkPipelineColorBlendAttachmentState, 3> blendAttachmentStates = {
			pipelineColorBlendAttachmentState,
			pipelineColorBlendAttachmentState,
			pipelineColorBlendAttachmentState
		};
		pipelineConfig.colorBlendInfo.attachmentCount = static_cast<uint32_t>(blendAttachmentStates.size());
		pipelineConfig.colorBlendInfo.pAttachments = blendAttachmentStates.data();

		defferedPipeline.offscreenPipeline = std::make_unique<EtherealPipeline>(
			etherealDevice,
			"shaders/deferred/mrt.vert.spv",
			"shaders/deferred/mrt.frag.spv",
			pipelineConfig);

		//Composition pipeline
		pipelineConfig.renderPass = compositionRenderPass;
		pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
		//clearing attachments(there will be a 1 fullscreen unit in VShared)
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();

		defferedPipeline.compositionPipeline = std::make_unique<EtherealPipeline>(
			etherealDevice,
			"shaders/deferred/composition.vert.spv",
			"shaders/deferred/composition.frag.spv",
			pipelineConfig);

	}

	void DefferedRenderSystem::writeCmpDescriptors(OffscreenFrameBuffer& offscreenFrmBuffer, 
		std::unique_ptr<EtherealBuffer> uboBuffer, 
		VkSampler& colorSampler) {
		
		compositionDescriptorPool = EtherealDescriptorPool::Builder(etherealDevice)
			.setMaxSets(1)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1) 
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3)
			.build();

		std::unique_ptr<EtherealDescriptorSetLayout> compositionDescriptorLayout = EtherealDescriptorSetLayout::Builder(etherealDevice)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		VkDescriptorImageInfo texDescriptorPosition{ colorSampler, offscreenFrmBuffer.position.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkDescriptorImageInfo texDescriptorNormal{ colorSampler, offscreenFrmBuffer.normal.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkDescriptorImageInfo texDescriptorAlbedo{ colorSampler, offscreenFrmBuffer.albedo.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

		auto bufferInfo = uboBuffer->descriptorInfo();
		EtherealDescriptorWriter(*compositionDescriptorLayout, *compositionDescriptorPool)
			.writeImage(1, &texDescriptorPosition)
			.writeImage(2, &texDescriptorNormal)
			.writeImage(3, &texDescriptorAlbedo)
			.writeBuffer(4, &bufferInfo)
			.build(compositionDescriptorSet);
	}

	void DefferedRenderSystem::bind(std::shared_ptr<EtherealModel> model,
		std::shared_ptr<EtherealTexture> texture,
		FrameInfo& frameInfo) {
		//bind model
		VkBuffer buffers[] = { model->vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(frameInfo.offscreenCmdBuffer, 0, 1, buffers, offsets);
		if (model->hasIndexBuffer) {
			vkCmdBindIndexBuffer(frameInfo.offscreenCmdBuffer, model->indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}

		//bind texture
		vkCmdBindDescriptorSets(frameInfo.offscreenCmdBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			1,
			1,
			&texture->getDescriptorSets()[frameInfo.frameIndex],
			0,
			nullptr);

		//bind composition descriptor set
		vkCmdBindDescriptorSets(frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			1,
			1,
			&compositionDescriptorSet,
			0,
			nullptr);
	}

	void DefferedRenderSystem::draw(FrameInfo& frameInfo, std::shared_ptr<EtherealModel> model) {
		if (model->hasIndexBuffer) {
			vkCmdDrawIndexed(frameInfo.offscreenCmdBuffer, model->indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(frameInfo.offscreenCmdBuffer, model->vertexCount, 1, 0, 0);
		}

		vkCmdDraw(frameInfo.commandBuffer, 3, 1, 0, 0);
	}
}