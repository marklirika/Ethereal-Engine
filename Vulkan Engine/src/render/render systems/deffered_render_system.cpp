#include "deffered_render_system.h"
#include "resources/ethereal_texture.h"
namespace ethereal {

	struct ModelData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	//Ubo Buffers is alreaddy writed I need to pull out all descriptor content to a Application function, its a better approach
	DefferedRenderSystem::DefferedRenderSystem(EtherealDevice& device,
		OffscreenFrameBuffer& offscreenFrmBuffer,
		VkRenderPass compositionRenderPass,
		VkDescriptorSetLayout globalSetLayout ) : etherealDevice{ device } {
		createColorSampler();
		writeCmpDescriptors(offscreenFrmBuffer);
		createSharedPipelineLayout(globalSetLayout);
		createPipelines(offscreenFrmBuffer.renderPass, compositionRenderPass);
	}

	void DefferedRenderSystem::createColorSampler() {
		VkSamplerCreateInfo sampler{};
		sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler.magFilter = VK_FILTER_NEAREST;
		sampler.minFilter = VK_FILTER_NEAREST;
		sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler.addressModeV = sampler.addressModeU;
		sampler.addressModeW = sampler.addressModeU;
		sampler.mipLodBias = 0.0f;
		sampler.maxAnisotropy = 1.0f;
		sampler.minLod = 0.0f;
		sampler.maxLod = 1.0f;
		sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		if (etherealDevice.device(), &sampler, nullptr, &colorSampler) {
			throw std::runtime_error("failed to create color sampler !");
		}
	}

	//this is a tip for doing textures in future
	void DefferedRenderSystem::writeCmpDescriptors(OffscreenFrameBuffer& offscreenFrmBuffer) {

		compositionDescriptorPool = EtherealDescriptorPool::Builder(etherealDevice)
			.setMaxSets(EtherealSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3 * EtherealSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		compositionDescriptorLayout = EtherealDescriptorSetLayout::Builder(etherealDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		VkDescriptorImageInfo texDescriptorPosition{ colorSampler, offscreenFrmBuffer.position.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkDescriptorImageInfo texDescriptorNormal{ colorSampler, offscreenFrmBuffer.normal.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkDescriptorImageInfo texDescriptorAlbedo{ colorSampler, offscreenFrmBuffer.albedo.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

		for (int i = 0; i < EtherealSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
			EtherealDescriptorWriter(*compositionDescriptorLayout, *compositionDescriptorPool)
				.writeImage(0, &texDescriptorPosition)
				.writeImage(1, &texDescriptorNormal)
				.writeImage(2, &texDescriptorAlbedo)
				.build(compositionDescriptorSets[i]);
		}
	}

	void DefferedRenderSystem::createSharedPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
		auto imageLayout = EtherealDescriptorSetLayout::Builder(etherealDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(ModelData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayout = { 
			globalSetLayout, 
			imageLayout->getDescriptorSetLayout(), 
			compositionDescriptorLayout->getDescriptorSetLayout() 
		};
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(etherealDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void DefferedRenderSystem::createPipelines(VkRenderPass offscreenRenderPass, VkRenderPass compositionRenderPass) {
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

		defferedPipelines.offscreenPipeline = std::make_unique<EtherealPipeline>(
			etherealDevice,
			"shaders/deffered/mrt.vert.spv",
			"shaders/deffered/mrt.frag.spv",
			pipelineConfig);

		//Composition pipeline
		pipelineConfig.renderPass = compositionRenderPass;
		pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_FRONT_BIT;

		pipelineConfig.colorBlendInfo.attachmentCount = 1;
		pipelineConfig.colorBlendInfo.pAttachments = &pipelineColorBlendAttachmentState;

		//clearing attachments(there will be a 1 fullscreen unit in VShared)
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();

		defferedPipelines.compositionPipeline = std::make_unique<EtherealPipeline>(
			etherealDevice,
			"shaders/deffered/deffered.vert.spv",
			"shaders/deffered/deffered.frag.spv",
			pipelineConfig);
	}

	void DefferedRenderSystem::bind(FrameInfo& frameInfo, 
		std::shared_ptr<EtherealModel> model,
		std::shared_ptr<EtherealTexture> texture) {
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

		vkCmdBindDescriptorSets(frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			2,
			1,
			&compositionDescriptorSets[frameInfo.frameIndex],
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
	
	void DefferedRenderSystem::render(FrameInfo& frameInfo) {
		//bind pipeline
		defferedPipelines.offscreenPipeline->bind(frameInfo.offscreenCmdBuffer);
		defferedPipelines.compositionPipeline->bind(frameInfo.commandBuffer);

		//bind ubos to offscreen and composition pipelines
		vkCmdBindDescriptorSets(frameInfo.offscreenCmdBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globaDescriptorSet,
			0,
			nullptr);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globaDescriptorSet,
			0,
			nullptr);

		auto view = frameInfo.scene.getRegistry().view<MeshComponent, TextureComponent, TransformComponent>();
		for (auto entity : view) {
			auto& mesh = view.get<MeshComponent>(entity);
			auto& texture = view.get<TextureComponent>(entity);
			auto& transform = view.get<TransformComponent>(entity);

			ModelData push{};
			push.modelMatrix = transform.mat4();
			push.normalMatrix = transform.normalMatrix();

			vkCmdPushConstants(frameInfo.offscreenCmdBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof(ModelData),
				&push);

			bind(frameInfo, mesh.model, texture.texture);
			draw(frameInfo, mesh.model);
		}
	}
}

