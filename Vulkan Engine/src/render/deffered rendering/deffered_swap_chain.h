#pragma once
#include "render/ethereal_swap_chain.h"

namespace ethereal {

	class DefferedSwapChain {
	public:
    	DefferedSwapChain(std::unique_ptr<EtherealSwapChain>& swapChain);
		~DefferedSwapChain() {}
		DefferedSwapChain() = delete;
		DefferedSwapChain(const DefferedSwapChain& swapChain) = delete;
		DefferedSwapChain& operator=(const DefferedSwapChain& swapChain) = delete;

		VkSampler colorSampler;
		VkSemaphore offscreenSemaphore = VK_NULL_HANDLE;
		OffscreenFrameBuffer offscreenFrmBuffer;
		std::unique_ptr<EtherealSwapChain>& etherealSwapChain;
	private:
		void createDefferedRenderPass();
		void writeDefferedFrmBuffer();
		void createColorSampler();
		void createSemaphore();
	};

} // namespace etheral