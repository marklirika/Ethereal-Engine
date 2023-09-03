#pragma once
#include "render/ethereal_swap_chain.h"

namespace ethereal {

	struct OffscreenFrameBuffer {
		int32_t width, height;
		VkFramebuffer frameBuffer;
		FrameBufferAttachment position, normal, albedo;
		FrameBufferAttachment depth;
		VkRenderPass renderPass;
	};

	class DefferedSwapChain : public EtherealSwapChain {
	public:
    	DefferedSwapChain(EtherealDevice& device, VkExtent2D windowExtent);
		DefferedSwapChain(EtherealDevice& device, VkExtent2D windowExtent, std::shared_ptr<EtherealSwapChain> previous);
		~DefferedSwapChain() {}

		DefferedSwapChain(const DefferedSwapChain& swapChain) = delete;
		DefferedSwapChain& operator=(const DefferedSwapChain& swapChain) = delete;

		OffscreenFrameBuffer& getOffscreenFrameBuffer() { return offscreenFrmBuffer; }
	private:
		void createOffscreenRenderPass();
		void writeOffscreenFrmBuffer();
		void createOffscreenSemaphores();		

		VkSemaphore offscreenSemaphore = VK_NULL_HANDLE;
		OffscreenFrameBuffer offscreenFrmBuffer;
	};

} // namespace etheral