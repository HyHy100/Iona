#include <Graphics/Vulkan/Helpers/QueueFamilies.hpp>

namespace iona::priv
{
    QueueFamilyIndices::QueueFamilyIndices()
        : graphicsFamily(std::numeric_limits<decltype(graphicsFamily)>::max()), 
            presentationFamily(std::numeric_limits<decltype(presentationFamily)>::max())
        {
        }

    bool QueueFamilyIndices::ok() const noexcept 
    {
        return graphicsFamily != std::numeric_limits<uint32_t>::max() && presentationFamily != std::numeric_limits<uint32_t>::max();
    }

    QueueFamilyIndices queryFamilyIndices(const ::vk::Instance& instance, const ::vk::PhysicalDevice& device) 
    {
        QueueFamilyIndices indices;
        
        auto vprops = device.getQueueFamilyProperties();

        std::for_each(vprops.begin(), vprops.end(), [&, i=0](::vk::QueueFamilyProperties& queue) 
        mutable {
            if (queue.queueFlags & ::vk::QueueFlagBits::eGraphics && glfwGetPhysicalDevicePresentationSupport(instance, device, i)) 
            {
                indices.presentationFamily = i;
                indices.graphicsFamily = i;
            }

            i++;
        });

        return indices;
    }
}