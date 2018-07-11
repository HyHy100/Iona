#include <Graphics/Vulkan/Helpers/Memory.hpp>
#include <Graphics/Vulkan/VKInfo.hpp>
#include <Exception/Exception.hpp>

namespace iona::priv {
     uint32_t getMemoryType(const vk::PhysicalDevice& device, uint32_t typeFilter, vk::MemoryPropertyFlags flags) 
     {
        auto props = device.getMemoryProperties();

        size_t i = 0;

        for (int i = 0; i < props.memoryTypeCount; i++) 
        {
            if ((props.memoryTypes[i].propertyFlags & flags) == flags) 
            {
                return i;
            }
            typeFilter >>= 0x1;
        }

        return std::numeric_limits<uint32_t>::max();
     }

    vk::DeviceMemory getDeviceOpMemory(std::size_t size)
    {
        static auto memLen = size;

        static auto memory = priv::VKInfo::device.allocateMemory(
            vk::MemoryAllocateInfo(
                size,
                priv::getMemoryType(priv::VKInfo::phyDevice, 0U, vk::MemoryPropertyFlagBits::eHostVisible)
            )
        );

        if (size > memLen)
        {
            priv::VKInfo::device.freeMemory(memory);

            memory = priv::VKInfo::device.allocateMemory(
                vk::MemoryAllocateInfo(
                    size, 
                    priv::getMemoryType(priv::VKInfo::phyDevice, 0U, vk::MemoryPropertyFlagBits::eHostVisible)
                )
            );

            memLen = size;
        }

        return memory;
    }
}