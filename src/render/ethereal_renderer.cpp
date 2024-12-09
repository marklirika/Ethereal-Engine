#include "ethereal_renderer.h"

//std
#include <stdexcept>;
#include <array>
#include <string>
#include <iostream>
#include <memory>

namespace ethereal {

	EtherealRenderer::EtherealRenderer(EtherealWindow& window, EtherealDevice& device) : etherealWindow{ window }, etherealDevice{device} {
		recreateSwapChain();
		createCommandBuffers();
	}

	EtherealRenderer::~EtherealRenderer() { freeCommandBuffers(); }

	void EtherealRenderer::recreateSwapChain() {
		auto extent = etherealWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = etherealWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(etherealDevice.device());
		etherealSwapChain = nullptr;

		if (etherealSwapChain == nullptr) {
			etherealSwapChain = std::make_unique<EtherealSwapChain>(etherealDevice, extent);
		} else {
			std::shared_ptr<EtherealSwapChain> oldSwapChain = std::move(etherealSwapChain);
			etherealSwapChain = std::make_unique<EtherealSwapChain>(etherealDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*etherealSwapChain.get())) {
				throw std::runtime_error("SwapChainImageFormat has changed");
			}
		}
	}

	void EtherealRenderer::createCommandBuffers() {
		commandBuffers.resize(EtherealSwapChain::MAX_FRAMES_IN_FLIGHT);
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
		currentFrameIndex = (currentFrameIndex + 1) % EtherealSwapChain::MAX_FRAMES_IN_FLIGHT;
		
	}
	void EtherealRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Cannot call beginSCRP, frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin render of a different frame");
		
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = etherealSwapChain->getRenderPass();
		renderPassInfo.framebuffer = etherealSwapChain->getFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = etherealSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
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

}
