#include <Graphics/Vulkan/Helpers/Transfer.hpp>
#include <Graphics/Vulkan/Helpers/CommandBuffer.hpp>

namespace iona::priv {
    void copyBufferToBuffer(const vk::Buffer& srcBuffer, 
                                    vk::Buffer& dstBuffer, 
                                    vk::DeviceSize size) 
    {
        auto cmdbf = priv::beginTempCommandBuffer();

        auto bfregion = vk::BufferCopy(vk::DeviceSize(0U), vk::DeviceSize(0U), size);
        cmdbf.copyBuffer(srcBuffer, dstBuffer, bfregion);

        priv::endTempCommandBuffer(cmdbf);
    }


    void copyBufferToImage(vk::Buffer src, 
                                    vk::Image dst, 
                                    SizeUint size) 
    {
        auto cmdbf = priv::beginTempCommandBuffer();

        auto region = vk::BufferImageCopy(
            vk::DeviceSize(0),
            0,
            0,
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1U),
            vk::Offset3D(0, 0, 0),
            vk::Extent3D(size.w, size.h, 1)
        );

        cmdbf.copyBufferToImage(src, dst, vk::ImageLayout::eTransferDstOptimal, region);

        priv::endTempCommandBuffer(cmdbf);
    }

    void layout2layout(const vk::Image& image, 
                                vk::Format format, 
                                vk::ImageLayout oldLayout, 
                                vk::ImageLayout newLayout) 
    {
        auto cmdbf = priv::beginTempCommandBuffer();

        auto barrier = vk::ImageMemoryBarrier(
            vk::AccessFlagBits(),
            vk::AccessFlags(),
            oldLayout,
            newLayout,
            VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED,
            image,
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0U, 1U, 0U, 1U)
        );

        cmdbf.pipelineBarrier(vk::PipelineStageFlags(), vk::PipelineStageFlags(), vk::DependencyFlags(), nullptr, nullptr, barrier);

        priv::endTempCommandBuffer(cmdbf);
    }
}