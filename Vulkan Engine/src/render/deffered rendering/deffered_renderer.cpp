#include "render/deffered rendering/deffered_renderer.h"

namespace ethereal {

	void DefferedRenderer::initDefferedPart() {
		if (offscreenCmdBuffer == VK_NULL_HANDLE) {
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = renderer.etherealDevice.getCommandPool();
			allocInfo.commandBufferCount = 1;
			if (vkAllocateCommandBuffers(renderer.etherealDevice.device(), &allocInfo, &offscreenCmdBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate offscreen command buffer!");
			}
		}
	}

	void DefferedRenderer::beginFrameDeffered() {
		VkCommandBufferBeginInfo cmdBufferBeginInfo{};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(offscreenCmdBuffer, &cmdBufferBeginInfo) != VK_SUCCESS) {
			throw std::runtime_error("frailed to begin offscreenCMDBuffer");
		}
	}

	void DefferedRenderer::endFrameDeffered() {
		if (vkEndCommandBuffer(offscreenCmdBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record buffer!");
		}
		vkResetCommandBuffer(offscreenCmdBuffer, 0);
	}

	void DefferedRenderer::beginSwapChainRenderPassDeffered() {
		// Clear values for all attachments written in the fragment shader
		std::array<VkClearValue, 4> clearValues;
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
		clearValues[1].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
		clearValues[2].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
		clearValues[3].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = defferedSwapChain->offscreenFrmBuffer.renderPass;
		renderPassBeginInfo.framebuffer = defferedSwapChain->offscreenFrmBuffer.frameBuffer;
		renderPassBeginInfo.renderArea.extent.width = defferedSwapChain->offscreenFrmBuffer.width;
		renderPassBeginInfo.renderArea.extent.height = defferedSwapChain->offscreenFrmBuffer.height;
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(offscreenCmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(defferedSwapChain->offscreenFrmBuffer.width);
		viewport.height = static_cast<float>(defferedSwapChain->offscreenFrmBuffer.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{ 
			{0, 0}, 
			{ defferedSwapChain->offscreenFrmBuffer.width, defferedSwapChain->offscreenFrmBuffer.height }
		};
		vkCmdSetViewport(offscreenCmdBuffer, 0, 1, &viewport);
		vkCmdSetScissor(offscreenCmdBuffer, 0, 1, &scissor);
	}

	void DefferedRenderer::endSwapChainRenderPassDeffered() {
		vkCmdEndRenderPass(offscreenCmdBuffer);
	}

}  // namespace ethereal