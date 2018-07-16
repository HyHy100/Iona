#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <algorithm>

namespace iona::priv 
{
    class QueueFamilyIndices 
    {
    public:
        uint32_t graphicsFamily;
        uint32_t presentationFamily;

        QueueFamilyIndices();

        bool ok() const noexcept;
    };

    QueueFamilyIndices queryFamilyIndices(const ::vk::Instance& instance, const ::vk::PhysicalDevice& device);
}