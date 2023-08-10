#pragma once

#include "core/ethereal_device.h" 
#include "render/ethereal_swap_chain.h"
#include "core/ethereal_window.h"

//std
#include <cassert>
#include <memory>
#include <vector>
#include <stdexcept>;
#include <array>
#include <string>
#include <iostream>
#include <memory>

namespace ethereal {

	class EtherealRenderer {

	public:
		EtherealRenderer(EtherealWindow& window, EtherealDevice& device);
		~EtherealRenderer();

		EtherealRenderer(const EtherealRenderer&) = delete;
		EtherealRenderer& operator=(const EtherealRenderer&) = delete;

		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(isFrameStarted && "Cannot get commandbuffer while frame not in progress");
			return commandBuffers[currentFrameIndex];
		}
		
		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame, its not in a progress");
			return currentFrameIndex;
		}
		VkRenderPass getSwapChainRenderPass() const { return etherealSwapChain->getRenderPass(); }
		float getAspectRation() const { return etherealSwapChain->extentAspectRatio(); }

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
		 
		EtherealWindow& etherealWindow;
		EtherealDevice& etherealDevice;
		std::unique_ptr<EtherealSwapChain> etherealSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;
		friend class DefferedRenderer;
	};
}