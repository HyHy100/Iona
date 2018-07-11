#pragma once

#include <vulkan/vulkan.hpp>

namespace iona::priv 
{
     uint32_t getMemoryType(const vk::PhysicalDevice& device, uint32_t typeFilter, vk::MemoryPropertyFlags flags);

     vk::DeviceMemory getDeviceOpMemory(std::size_t size);
}