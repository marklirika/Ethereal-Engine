#include "render/deffered rendering/deffered_swap_chain.h"

namespace ethereal {
	
	DefferedSwapChain::DefferedSwapChain(std::unique_ptr<EtherealSwapChain>& swapChain)
		: etherealSwapChain{ swapChain } {
		createDefferedRenderPass();
		writeDefferedFrmBuffer();
		createColorSampler();
		createSemaphore();
	}

	void DefferedSwapChain::createDefferedRenderPass() {
		offscreenFrmBuffer.width = etherealSwapChain->swapChainExtent.width;
		offscreenFrmBuffer.height = etherealSwapChain->swapChainExtent.height;

		// (World space) Positions
		etherealSwapChain->createAttachment(
			VK_FORMAT_R16G16B16A16_SFLOAT, 
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
			offscreenFrmBuffer.position);

		// (World space) Normals
		etherealSwapChain->createAttachment(
			VK_FORMAT_R16G16B16A16_SFLOAT,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			offscreenFrmBuffer.normal);

		// Albedo (color)
		etherealSwapChain->createAttachment(
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			offscreenFrmBuffer.albedo);

		// Depth attachment
		VkFormat depthFormat = etherealSwapChain->findDepthFormat();
		etherealSwapChain->createAttachment(
			depthFormat,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			offscreenFrmBuffer.depth);

		// Set up separate renderpass with references to the color and depth attachments
		std::array<VkAttachmentDescription, 4> attachmentDescs = {};

		// Init attachment properties
		for (uint32_t i = 0; i < 4; ++i) {
			attachmentDescs[i].samples = VK_SAMPLE_COUNT_1_BIT;
			attachmentDescs[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachmentDescs[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentDescs[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDescs[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			if (i == 3)
			{
				attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
			else
			{
				attachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				attachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
		}

		// Formats
		attachmentDescs[0].format = offscreenFrmBuffer.position.format;
		attachmentDescs[1].format = offscreenFrmBuffer.normal.format;
		attachmentDescs[2].format = offscreenFrmBuffer.albedo.format;
		attachmentDescs[3].format = offscreenFrmBuffer.depth.format;

		std::vector<VkAttachmentReference> colorReferences;
		colorReferences.push_back({ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
		colorReferences.push_back({ 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
		colorReferences.push_back({ 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

		//ÂÀÆÍÎ
		VkAttachmentReference depthReference = {};
		depthReference.attachment = 3;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.pColorAttachments = colorReferences.data();
		subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
		subpass.pDepthStencilAttachment = &depthReference;

		// Use subpass dependencies for attachment layout transitions
		std::array<VkSubpassDependency, 2> dependencies{};

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.pAttachments = attachmentDescs.data();
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescs.size());
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 2;
		renderPassInfo.pDependencies = dependencies.data();

		if (vkCreateRenderPass(etherealSwapChain->etherealDevice.device(),
			&renderPassInfo, 
			nullptr, 
			&offscreenFrmBuffer.renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void DefferedSwapChain::writeDefferedFrmBuffer() {
		std::array<VkImageView, 4> attachments;
		attachments[0] = offscreenFrmBuffer.position.view;
		attachments[1] = offscreenFrmBuffer.normal.view;
		attachments[2] = offscreenFrmBuffer.albedo.view;
		attachments[3] = offscreenFrmBuffer.depth.view;

		VkFramebufferCreateInfo fbufCreateInfo{};
		fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbufCreateInfo.pNext = NULL;
		fbufCreateInfo.renderPass = offscreenFrmBuffer.renderPass;
		fbufCreateInfo.pAttachments = attachments.data();
 		fbufCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		fbufCreateInfo.width = offscreenFrmBuffer.width;
		fbufCreateInfo.height = offscreenFrmBuffer.height;
		fbufCreateInfo.layers = 1;
		if (vkCreateFramebuffer(etherealSwapChain->etherealDevice.device(),
			&fbufCreateInfo,
			nullptr,
			&offscreenFrmBuffer.frameBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to write frame buffer!");
		}
	}

	void DefferedSwapChain::createColorSampler() {
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
		if (vkCreateSampler(etherealSwapChain->etherealDevice.device(), &sampler, nullptr, &colorSampler)) {
			throw std::runtime_error("failed to create color sampler !");
		}
	}

	void DefferedSwapChain::createSemaphore() {
		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		if (vkCreateSemaphore(etherealSwapChain->etherealDevice.device(), &semaphoreCreateInfo, nullptr, &offscreenSemaphore) != VK_SUCCESS) {
			throw std::runtime_error("failed to create offscreen semaphore");
		}
	}

}  // namespace ethereal