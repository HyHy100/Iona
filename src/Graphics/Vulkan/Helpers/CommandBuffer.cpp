#include <Graphics/Vulkan/Helpers/CommandBuffer.hpp>

namespace iona::priv {
    vk::CommandBuffer beginTempCommandBuffer()
    {
        vk::CommandBuffer cmdbf = VkEnv::device.logical.allocateCommandBuffers(vk::CommandBufferAllocateInfo(
            priv::VkEnv::commands.pool,
            vk::CommandBufferLevel::ePrimary,
            1U
        )).front();

        cmdbf.begin(
            vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit)
        );

        return cmdbf;
    }

    void endTempCommandBuffer(vk::CommandBuffer& cmdbf)
    {
        cmdbf.end();

        auto fence = priv::VkEnv::device.logical.createFence(
            vk::FenceCreateInfo()
        );

        priv::VkEnv::queues.graphics.submit(vk::SubmitInfo(
            0U,
            nullptr,
            nullptr,
            1U,
            &cmdbf,
            0U,
            nullptr
        ), fence);

        priv::VkEnv::device.logical.waitForFences(fence, VK_TRUE, std::numeric_limits<std::size_t>::max());

        priv::VkEnv::device.logical.freeCommandBuffers(priv::VkEnv::commands.pool, cmdbf);
    }
}