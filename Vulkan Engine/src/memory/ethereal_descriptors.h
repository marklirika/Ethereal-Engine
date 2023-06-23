#pragma once

#pragma once

#include "../core/ethereal_device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace ethereal {

    class EtherealDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(EtherealDevice& etherealDevice) : etherealDevice{ etherealDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<EtherealDescriptorSetLayout> build() const;

        private:
            EtherealDevice& etherealDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        EtherealDescriptorSetLayout(
            EtherealDevice& etherealDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~EtherealDescriptorSetLayout();

        EtherealDescriptorSetLayout(const EtherealDescriptorSetLayout&) = delete;
        EtherealDescriptorSetLayout& operator=(const EtherealDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        EtherealDevice& etherealDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class EtherealDescriptorWriter;
    };

    class EtherealDescriptorPool {
    public:
        class Builder {
        public:
            Builder(EtherealDevice& etherealDevice) : etherealDevice{ etherealDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<EtherealDescriptorPool> build() const;

        private:
            EtherealDevice& etherealDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        EtherealDescriptorPool(
            EtherealDevice& ethrealDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~EtherealDescriptorPool();
        EtherealDescriptorPool(const EtherealDescriptorPool&) = delete;
        EtherealDescriptorPool& operator=(const EtherealDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        EtherealDevice& etherealDevice;
        VkDescriptorPool descriptorPool;

        friend class EtherealDescriptorWriter;
    };

    class EtherealDescriptorWriter {
    public:
        EtherealDescriptorWriter(EtherealDescriptorSetLayout& setLayout, EtherealDescriptorPool& pool);

        EtherealDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        EtherealDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        EtherealDescriptorSetLayout& setLayout;
        EtherealDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}