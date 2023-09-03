#include "ethereal_renderer.h"

namespace ethereal {

	EtherealRenderer::EtherealRenderer(EtherealWindow& window, EtherealDevice& device) : etherealWindow{ window }, etherealDevice{ device } {
		recreateSwapChain();
		createCommandBuffers();
	}

	EtherealRenderer::~EtherealRenderer() { 
		freeCommandBuffers(); 
#ifdef DEFFERED_MODE
		freeOffscreenCmdBuffers();
#endif
	}

	void EtherealRenderer::recreateSwapChain() {
		auto extent = etherealWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = etherealWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(etherealDevice.device());
		etherealSwapChain = nullptr;

		if (etherealSwapChain == nullptr) {
			etherealSwapChain = std::make_unique<SwapChain>(etherealDevice, extent);
		} 
		else {
			std::shared_ptr<SwapChain> oldSwapChain = std::move(etherealSwapChain);
			etherealSwapChain = std::make_unique<SwapChain>(etherealDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*etherealSwapChain.get())) {
				throw std::runtime_error("SwapChainImageFormat has changed");
			}
		}
	}

	void EtherealRenderer::createCommandBuffers() {
		commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = etherealDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(etherealDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void EtherealRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(etherealDevice.device(), etherealDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer EtherealRenderer::beginFrame() {
		assert(!isFrameStarted && "Can't call while beginFrame already in progress");
		auto result = etherealSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image");
		}
		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording!");
		}
		return commandBuffer;
	}

	void EtherealRenderer::endFrame() {
		assert(isFrameStarted && "Frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record buffer!");
		}

		auto result = etherealSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || etherealWindow.wasWindowResized()) {
			etherealWindow.resetWindowResizedFlag();
			recreateSwapChain();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
		
	}

	void EtherealRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cannot call beginSCRP, frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin render of a different frame");
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = etherealSwapChain->getRenderPass();
		renderPassInfo.framebuffer = etherealSwapChain->getFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = etherealSwapChain->getSwapChainExtent();
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(etherealSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(etherealSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, etherealSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void EtherealRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cannot call endSCRP, frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end render of a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}

#ifdef DEFFERED_MODE
	void EtherealRenderer::freeOffscreenCmdBuffers() {
		vkFreeCommandBuffers(etherealDevice.device(), etherealDevice.getCommandPool(), static_cast<uint32_t>(offscreenCmdBuffers.size()), offscreenCmdBuffers.data());
		offscreenCmdBuffers.clear();
	}

	void EtherealRenderer::createOffscreenCmdBuffers() {
		offscreenCmdBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = etherealDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(offscreenCmdBuffers.size());

		if (vkAllocateCommandBuffers(etherealDevice.device(), &allocInfo, offscreenCmdBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void EtherealRenderer::beginOffscreenFrame() {
		auto offscreenCmdBuffer = getCurrentOffscreenCmdBuffer();
		VkCommandBufferBeginInfo cmdBufferBeginInfo{};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(offscreenCmdBuffer, &cmdBufferBeginInfo) != VK_SUCCESS) {
			throw std::runtime_error("frailed to begin offscreenCMDBuffer");
		}
	}

	void EtherealRenderer::endOffscreenFrame() {
		auto offscreenCmdBuffer = getCurrentOffscreenCmdBuffer();
		if (vkEndCommandBuffer(offscreenCmdBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record buffer!");
		}
	}
	void EtherealRenderer::beginSwapChainRenderPass(VkCommandBuffer offscreenCmdBuffer, VkCommandBuffer commandBuffer) {
		// Clear values for all attachments written in the fragment shader
		std::array<VkClearValue, 4> clearValues;
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
		clearValues[1].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
		clearValues[2].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
		clearValues[3].depthStencil = { 1.0f, 0 };
		auto& offscreenFrameBuffer = etherealSwapChain->getOffscreenFrameBuffer();
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = offscreenFrameBuffer.renderPass;
		renderPassBeginInfo.framebuffer = offscreenFrameBuffer.frameBuffer;
		renderPassBeginInfo.renderArea.extent.width = offscreenFrameBuffer.width;
		renderPassBeginInfo.renderArea.extent.height = offscreenFrameBuffer.height;
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(offscreenCmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(offscreenFrameBuffer.width);
		viewport.height = static_cast<float>(offscreenFrameBuffer.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{
			{0, 0},
			{ offscreenFrameBuffer.width, offscreenFrameBuffer.height }
		};
		vkCmdSetViewport(offscreenCmdBuffer, 0, 1, &viewport);
		vkCmdSetScissor(offscreenCmdBuffer, 0, 1, &scissor);
		beginSwapChainRenderPass(commandBuffer);
	}

	void EtherealRenderer::endSwapChainRenderPass(VkCommandBuffer offscreenCmdBuffer, VkCommandBuffer commandBuffer) {
		vkCmdEndRenderPass(offscreenCmdBuffer);
		endSwapChainRenderPass(commandBuffer);
	}
#endif
}
