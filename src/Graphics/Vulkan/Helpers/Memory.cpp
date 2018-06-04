#include <Graphics/Vulkan/Helpers/Memory.hpp>
#include <Exception/Exception.hpp>

namespace iona::priv {
     uint32_t getMemoryType(const vk::PhysicalDevice& device, vk::MemoryPropertyFlags flags) {
        auto props = device.getMemoryProperties();

        size_t i = 0;

        for (; i < props.memoryTypeCount; i++) {
            const auto type = props.memoryTypes[i];
            if (type.propertyFlags & flags) {
                return i;
            }
        }

        if (i == props.memoryTypeCount) {
            throw Exception("no memory type '%s' found");
        }

        return std::numeric_limits<uint32_t>::max();
    }
}