#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace iona::priv {
    class SwapchainSupportDetails {
    public:
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    SwapchainSupportDetails querySwapchainSupport(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) {
        SwapchainSupportDetails details;

        details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
        details.formats = device.getSurfaceFormatsKHR(surface);
        details.presentModes = device.getSurfacePresentModesKHR(surface);

        assert(details.formats.size() != 0);
        assert(details.presentModes.size() != 0);

        return details;
    }
}