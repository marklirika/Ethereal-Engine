#include "ethereal_swap_chain.h"

namespace ethereal {

    EtherealSwapChain::EtherealSwapChain(EtherealDevice &deviceRef, VkExtent2D extent)
        : etherealDevice{deviceRef}, windowExtent{extent} {
        init();
    }

    EtherealSwapChain::EtherealSwapChain(EtherealDevice& deviceRef, VkExtent2D extent, std::shared_ptr<EtherealSwapChain> previous)
        : etherealDevice{ deviceRef }, windowExtent{ extent }, oldSwapChain{ previous } {
        init();

        //clean up old swapchain
        oldSwapChain = nullptr;
    }

    void EtherealSwapChain::init() {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
        createSyncObjects();
    }
    
    EtherealSwapChain::~EtherealSwapChain() {
      for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(etherealDevice.device(), imageView, nullptr);
      }
      swapChainImageViews.clear();

      if (swapChain != nullptr) {
        vkDestroySwapchainKHR(etherealDevice.device(), swapChain, nullptr);
        swapChain = nullptr;
      }

      for (int i = 0; i < depthImages.size(); i++) {
        vkDestroyImageView(etherealDevice.device(), depthImageViews[i], nullptr);
        vkDestroyImage(etherealDevice.device(), depthImages[i], nullptr);
        vkFreeMemory(etherealDevice.device(), depthImageMemory[i], nullptr);
      }

      for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(etherealDevice.device(), framebuffer, nullptr);
      }

      vkDestroyRenderPass(etherealDevice.device(), renderPass, nullptr);

      // cleanup synchronization objects
      for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(etherealDevice.device(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(etherealDevice.device(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(etherealDevice.device(), inFlightFences[i], nullptr);
      }
    }

    VkResult EtherealSwapChain::acquireNextImage(uint32_t *imageIndex) {
      vkWaitForFences(
          etherealDevice.device(),
          1,
          &inFlightFences[currentFrame],
          VK_TRUE,
          std::numeric_limits<uint64_t>::max());

      VkResult result = vkAcquireNextImageKHR(
          etherealDevice.device(),
          swapChain,
          std::numeric_limits<uint64_t>::max(),
          imageAvailableSemaphores[currentFrame],  // must be a not signaled semaphore
          VK_NULL_HANDLE,
          imageIndex);

      return result;
    }

    VkResult EtherealSwapChain::submitCommandBuffers(
        const VkCommandBuffer *buffers, uint32_t *imageIndex) {
      if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(etherealDevice.device(), 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
      }
      imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

      VkSubmitInfo submitInfo = {};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

      VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
      VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = waitSemaphores;
      submitInfo.pWaitDstStageMask = waitStages;

      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = buffers;

      VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = signalSemaphores;

      vkResetFences(etherealDevice.device(), 1, &inFlightFences[currentFrame]);
      if (vkQueueSubmit(etherealDevice.graphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) !=
          VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
      }

      VkPresentInfoKHR presentInfo = {};
      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

      presentInfo.waitSemaphoreCount = 1;
      presentInfo.pWaitSemaphores = signalSemaphores;

      VkSwapchainKHR swapChains[] = {swapChain};
      presentInfo.swapchainCount = 1;
      presentInfo.pSwapchains = swapChains;

      presentInfo.pImageIndices = imageIndex;

      auto result = vkQueuePresentKHR(etherealDevice.presentQueue(), &presentInfo);

      currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

      return result;
    }

    void EtherealSwapChain::createSwapChain() {
      SwapChainSupportDetails swapChainSupport = etherealDevice.getSwapChainSupport();

      VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
      VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
      VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

      uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
      if (swapChainSupport.capabilities.maxImageCount > 0 &&
          imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
      }

      VkSwapchainCreateInfoKHR createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      createInfo.surface = etherealDevice.surface();

      createInfo.minImageCount = imageCount;
      createInfo.imageFormat = surfaceFormat.format;
      createInfo.imageColorSpace = surfaceFormat.colorSpace;
      createInfo.imageExtent = extent;
      createInfo.imageArrayLayers = 1;
      createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

      QueueFamilyIndices indices = etherealDevice.findPhysicalQueueFamilies();
      uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

      if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
      } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;      // Optional
        createInfo.pQueueFamilyIndices = nullptr;  // Optional
      }

      createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
      createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

      createInfo.presentMode = presentMode;
      createInfo.clipped = VK_TRUE;

      createInfo.oldSwapchain == nullptr ? VK_NULL_HANDLE : oldSwapChain->swapChain;

      if (vkCreateSwapchainKHR(etherealDevice.device(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
      }

      // we only specified a minimum number of images in the swap chain, so the implementation is
      // allowed to create a swap chain with more. That's why we'll first query the final number of
      // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
      // retrieve the handles.
      vkGetSwapchainImagesKHR(etherealDevice.device(), swapChain, &imageCount, nullptr);
      swapChainImages.resize(imageCount);
      vkGetSwapchainImagesKHR(etherealDevice.device(), swapChain, &imageCount, swapChainImages.data());

      swapChainImageFormat = surfaceFormat.format;
      swapChainExtent = extent;
    }

    void EtherealSwapChain::createImageViews() {
      swapChainImageViews.resize(swapChainImages.size());
      for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swapChainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = swapChainImageFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(etherealDevice.device(), &viewInfo, nullptr, &swapChainImageViews[i]) !=
            VK_SUCCESS) {
          throw std::runtime_error("failed to create texture image view!");
        }
      }
    }

    void EtherealSwapChain::createRenderPass() {

      VkAttachmentDescription depthAttachment{};
      depthAttachment.format = findDepthFormat();
      depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
      depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      VkAttachmentReference depthAttachmentRef{};
      depthAttachmentRef.attachment = 1;
      depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      VkAttachmentDescription colorAttachment = {};
      colorAttachment.format = getSwapChainImageFormat();
      colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
      colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

      VkAttachmentReference colorAttachmentRef = {};
      colorAttachmentRef.attachment = 0;
      colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
      VkSubpassDescription subpass = {};
      subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpass.colorAttachmentCount = 1;
      subpass.pColorAttachments = &colorAttachmentRef;
      subpass.pDepthStencilAttachment = &depthAttachmentRef;

      VkSubpassDependency dependency = {};
      dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
      dependency.srcAccessMask = 0;
      dependency.srcStageMask =
          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
      dependency.dstSubpass = 0;
      dependency.dstStageMask =
          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
      dependency.dstAccessMask =
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

      std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
      VkRenderPassCreateInfo renderPassInfo = {};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
      renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
      renderPassInfo.pAttachments = attachments.data();
      renderPassInfo.subpassCount = 1;
      renderPassInfo.pSubpasses = &subpass;
      renderPassInfo.dependencyCount = 1;
      renderPassInfo.pDependencies = &dependency;

      if (vkCreateRenderPass(etherealDevice.device(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
      }
    }

    void EtherealSwapChain::createFramebuffers() {
      swapChainFramebuffers.resize(imageCount());
      for (size_t i = 0; i < imageCount(); i++) {
        std::array<VkImageView, 2> attachments = {swapChainImageViews[i], depthImageViews[i]};

        VkExtent2D swapChainExtent = getSwapChainExtent();
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(
                etherealDevice.device(),
                &framebufferInfo,
                nullptr,
                &swapChainFramebuffers[i]) != VK_SUCCESS) {
          throw std::runtime_error("failed to create framebuffer!");
        }
      }
    }

    void EtherealSwapChain::createDepthResources() {
      VkFormat depthFormat = findDepthFormat();
      swapChainDepthFormat = depthFormat;
      VkExtent2D swapChainExtent = getSwapChainExtent();

      depthImages.resize(imageCount());
      depthImageMemory.resize(imageCount());
      depthImageViews.resize(imageCount());

      for (int i = 0; i < depthImages.size(); i++) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = swapChainExtent.width;
        imageInfo.extent.height = swapChainExtent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = depthFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0;

        etherealDevice.createImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            depthImages[i],
            depthImageMemory[i]);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = depthImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = depthFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(etherealDevice.device(), &viewInfo, nullptr, &depthImageViews[i]) != VK_SUCCESS) {
          throw std::runtime_error("failed to create texture image view!");
        }
      }
    }

    void EtherealSwapChain::createSyncObjects() {
      imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
      renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
      inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
      imagesInFlight.resize(imageCount(), VK_NULL_HANDLE);

      VkSemaphoreCreateInfo semaphoreInfo = {};
      semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

      VkFenceCreateInfo fenceInfo = {};
      fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

      for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(etherealDevice.device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
            vkCreateSemaphore(etherealDevice.device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) !=
                VK_SUCCESS ||
            vkCreateFence(etherealDevice.device(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
          throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
      }
    }

    VkSurfaceFormatKHR EtherealSwapChain::chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats) {
      for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
          return availableFormat;
        }
      }

      return availableFormats[0];
    }

    VkPresentModeKHR EtherealSwapChain::chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes) {
      for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
          std::cout << "Present mode: Mailbox" << std::endl;
          return availablePresentMode;
        }
      }

      // for (const auto &availablePresentMode : availablePresentModes) {
      //   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      //     std::cout << "Present mode: Immediate" << std::endl;
      //     return availablePresentMode;
      //   }
      // }

      std::cout << "Present mode: V-Sync" << std::endl;
      return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D EtherealSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
      if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
      } else {
        VkExtent2D actualExtent = windowExtent;
        actualExtent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
      }
    }

    void EtherealSwapChain::createAttachment(VkFormat format, VkImageUsageFlagBits usage, FrameBufferAttachment* attachment)
    {
        VkImageAspectFlags aspectMask = 0;
        VkImageLayout imageLayout;

        attachment->format = format;

        if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            if (format >= VK_FORMAT_D16_UNORM_S8_UINT)
                aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        assert(aspectMask > 0 && "Aspect Mask > 0");

        VkImageCreateInfo image{};
        image.imageType = VK_IMAGE_TYPE_2D;
        image.format = format;
        image.extent.width = getSwapChainExtent().width;
        image.extent.height = getSwapChainExtent().height;
        image.extent.depth = 1;
        image.mipLevels = 1;
        image.arrayLayers = 1;
        image.samples = VK_SAMPLE_COUNT_1_BIT;
        image.tiling = VK_IMAGE_TILING_OPTIMAL;
        image.usage = usage | VK_IMAGE_USAGE_SAMPLED_BIT;

        VkMemoryAllocateInfo memAlloc{};
        VkMemoryRequirements memReqs;

        if (vkCreateImage(etherealDevice.device(), &image, nullptr, &attachment->image) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create attachment image!");
        }

        vkGetImageMemoryRequirements(etherealDevice.device(), attachment->image, &memReqs);
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex = etherealDevice.findMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        assert(vkAllocateMemory(etherealDevice.device(), &memAlloc, nullptr, &attachment->mem) && "failed to alloc memory");
        assert(vkBindImageMemory(etherealDevice.device(), attachment->image, attachment->mem, 0) && "failed to bind memory");

        VkImageViewCreateInfo imageView{};
        imageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageView.format = format;
        imageView.subresourceRange = {};
        imageView.subresourceRange.aspectMask = aspectMask;
        imageView.subresourceRange.baseMipLevel = 0;
        imageView.subresourceRange.levelCount = 1;
        imageView.subresourceRange.baseArrayLayer = 0;
        imageView.subresourceRange.layerCount = 1;
        imageView.image = attachment->image;

        if (vkCreateImageView(etherealDevice.device(), &imageView, nullptr, &attachment->view) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    VkFormat EtherealSwapChain::findDepthFormat() {
      return etherealDevice.findSupportedFormat(
          {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
          VK_IMAGE_TILING_OPTIMAL,
          VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

}  // namespace ethereal
