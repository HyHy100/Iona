#include <Graphics/Vulkan/Helpers/CommandBuffer.hpp>

namespace iona::priv {
    vk::CommandBuffer beginTempCommandBuffer() {
        vk::CommandBuffer cmdbf = VKInfo::device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(
            priv::VKInfo::commandPool,
            vk::CommandBufferLevel::ePrimary,
            1U
        )).front();

        cmdbf.begin(
            vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit)
        );

        return cmdbf;
    }

    void endTempCommandBuffer(vk::CommandBuffer& cmdbf) {
        cmdbf.end();

        auto fence = priv::VKInfo::device.createFence(vk::FenceCreateInfo());
        fprintf(stderr, "submitting... ");
        priv::VKInfo::graphicsQueue.submit(vk::SubmitInfo(
            0,
            nullptr,
            nullptr,
            1U,
            &cmdbf,
            0,
            nullptr
        ), fence);

        fprintf(stderr, "submitted.\n");

        priv::VKInfo::device.waitForFences(fence, VK_TRUE, std::numeric_limits<uint16_t>::max());

        priv::VKInfo::device.destroyFence(fence);

        priv::VKInfo::device.freeCommandBuffers(priv::VKInfo::commandPool, cmdbf);

        priv::VKInfo::device.waitIdle();
    }
}