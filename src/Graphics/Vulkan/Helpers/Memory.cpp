#include <Graphics/Vulkan/Helpers/Memory.hpp>
#include <Exception/Exception.hpp>

#include <iostream>

namespace iona::priv {
     uint32_t getMemoryType(const vk::PhysicalDevice& device, uint32_t typeFilter, vk::MemoryPropertyFlags flags) {
        auto props = device.getMemoryProperties();

        size_t i = 0;

        for (int i = 0; i < props.memoryTypeCount; i++) {
            if ((0x1 & typeFilter) == typeFilter && (props.memoryTypes[i].propertyFlags & flags) == flags) {
                std::cout << "found" << std::endl;
                return i;
            }
            typeFilter >>= 0x1;
        }

        return std::numeric_limits<uint32_t>::max();
    }
}