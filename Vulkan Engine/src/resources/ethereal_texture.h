#pragma once
#include <vulkan/vulkan_core.h>
#include "core/ethereal_device.h"
#include "render/ethereal_swap_chain.h"
#include "memory/ethereal_descriptors.h"
#include <string.h>
namespace ethereal {
    class EtherealTexture {
    public:
        EtherealTexture(EtherealDevice& device, const std::string& filepath);
        ~EtherealTexture();

        EtherealTexture(const EtherealTexture&) = delete;
        EtherealTexture& operator=(const EtherealTexture&) = delete;
        
        VkSampler getSampler() { return sampler; }
        VkImageView getImageView() { return imageView; }
        VkImageLayout getImageLayout() { return imageLayout; }
        std::vector<VkDescriptorSet> getDescriptorSets() { return descriptorSets; }
    private:
        void writeDescriptors();
        void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
        void generateMipmaps();

        int width, height, mipLevels;
        VkImage image;
        VkImageView imageView;
        VkSampler sampler;
        VkFormat imageFormat;
        VkImageLayout imageLayout;

        std::unique_ptr<EtherealDescriptorPool> descriptorPool;
        std::unique_ptr<EtherealDescriptorSetLayout> descriptorSetLayout;
        std::vector<VkDescriptorSet> descriptorSets { EtherealSwapChain::MAX_FRAMES_IN_FLIGHT };
        
        VkDeviceMemory imageMemory;
        EtherealDevice& etherealDevice;
    };
}