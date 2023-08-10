#pragma once
#include "render/ethereal_swap_chain.h"
#include "memory/ethereal_frame_info.h"
namespace ethereal {

	struct DefferedSwapChain {
	public:
		DefferedSwapChain(std::unique_ptr<EtherealSwapChain>& swapChain);
		~DefferedSwapChain();
		DefferedSwapChain() = delete;
		DefferedSwapChain(const DefferedSwapChain& swapChain) = delete;
		DefferedSwapChain& operator=(const DefferedSwapChain& swapChain) = delete;
		DefferedSwapChain(const DefferedSwapChain&& swapChain) = delete;
		DefferedSwapChain& operator=(const DefferedSwapChain&& swapChain) = delete;

		OffscreenFrameBuffer offscreenFrmBuffer;
		VkSampler colorSampler;
		VkSemaphore offscreenSemaphore = VK_NULL_HANDLE;
		std::unique_ptr<EtherealSwapChain>& etherealSwapChain;
	private:
		void createDefferedRenderPass();
		void writeDefferedFrmBuffer();
		void createColorSampler();
		void createSemaphore();
	};

} // namespace etheral