#pragma once

#include <vulkan/vulkan.hpp>
#include <Generic/Size.hpp>

namespace iona::priv {
    void copyBufferToBuffer(const vk::Buffer& srcBuffer, 
                                    vk::Buffer& dstBuffer, 
                                    vk::DeviceSize size);


    void copyBufferToImage(vk::Buffer src, 
                                    vk::Image dst, 
                                    SizeUint size);

    void layout2layout(const vk::Image& image, 
                                vk::Format format, 
                                vk::ImageLayout oldLayout, 
                                vk::ImageLayout newLayout);
}
