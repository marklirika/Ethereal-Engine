#pragma once
#include "core/ethereal_device.h" 
#include "render/ethereal_swap_chain.h"
#include "core/ethereal_window.h"
#include "render/ethereal_renderer.h"
#include "memory/ethereal_buffer.h"
#include "deffered_swap_chain.h"

//std
#include <cassert>
#include <memory>
#include <vector>
#include <functional>

namespace ethereal {

	struct DefferedRenderer {
		DefferedRenderer(EtherealRenderer& renderer);
		~DefferedRenderer() {}
		DefferedRenderer(const DefferedRenderer&) = delete;
		DefferedRenderer& operator=(const DefferedRenderer&) = delete;
		DefferedRenderer(DefferedRenderer&&) = delete;
		DefferedRenderer& operator=(DefferedRenderer&&) = delete;
		void init();
		void beginFrameDeffered();
		void endFrameDeffered();
		void beginSwapChainRenderPassDeffered();
		void endSwapChainRenderPassDeffered();
		
		EtherealRenderer& renderer;
		VkCommandBuffer offscreenCmdBuffer = VK_NULL_HANDLE;
		std::unique_ptr<DefferedSwapChain> defferedSwapChain;
	};

} // namespace ethereal