#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <algorithm>

namespace iona::priv {
    class QueueFamilyIndices {
    public:
        uint32_t graphicsFamily = std::numeric_limits<uint32_t>::max();
        uint32_t presentationFamily = std::numeric_limits<uint32_t>::max();

        bool ok();
    };

    QueueFamilyIndices queryFamilyIndices(const ::vk::Instance& instance, const ::vk::PhysicalDevice& device);
}