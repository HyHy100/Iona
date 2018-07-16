#pragma once

#include <mutex>

#include <vulkan/vulkan.hpp>

#include <Graphics/VertexList.hpp>
#include <Graphics/Vulkan/VKInfo.hpp>
#include <Graphics/Vulkan/Helpers/Memory.hpp>
#include <Graphics/Vulkan/Helpers/Transfer.hpp>

namespace iona 
{
    class VertexBuffer 
    {
    public:
        VertexBuffer() = default;

        template<
            typename T
        >
        explicit VertexBuffer(const T& vlist)
        {
            if (m_buffer) 
            {
                priv::VkEnv::device.logical.destroyBuffer(m_buffer);
            }

            if (m_memory)
            {
                priv::VkEnv::device.logical.freeMemory(m_memory);
            }

            const std::size_t memLen = vlist.size() * sizeof(Vertex);

            static std::mutex mutex;
            
            vk::Buffer stagingBuffer = priv::VkEnv::device.logical.createBuffer(
                vk::BufferCreateInfo(
                    vk::BufferCreateFlags(),
                    memLen,
                    vk::BufferUsageFlagBits::eTransferSrc,
                    vk::SharingMode::eExclusive
                )
            );

            auto requirements = priv::VkEnv::device.logical.getBufferMemoryRequirements(stagingBuffer);

            const vk::DeviceMemory stagingMemory = priv::getDeviceOpMemory(requirements.size);

            priv::VkEnv::device.logical.bindBufferMemory(stagingBuffer, stagingMemory, 0U);

            mutex.lock();

            Vertex* const data = reinterpret_cast<Vertex*>(priv::VkEnv::device.logical.mapMemory(stagingMemory, 0U, requirements.size));

            std::memcpy(data, vlist.data(), memLen);

            priv::VkEnv::device.logical.unmapMemory(stagingMemory);

            priv::VkEnv::device.logical.flushMappedMemoryRanges(
                vk::MappedMemoryRange(
                    stagingMemory, 
                    0U, 
                    memLen
                )
            );

            priv::VkEnv::device.logical.invalidateMappedMemoryRanges(
                vk::MappedMemoryRange(
                    stagingMemory, 
                    0U, 
                    memLen
                )
            );

            mutex.unlock();

            m_buffer = priv::VkEnv::device.logical.createBuffer(
                vk::BufferCreateInfo(
                    vk::BufferCreateFlags(),
                    memLen,
                    vk::BufferUsageFlagBits::eTransferDst,
                    vk::SharingMode::eExclusive                
                )
            );

            auto reqs = priv::VkEnv::device.logical.getBufferMemoryRequirements(m_buffer);

            m_memory = priv::VkEnv::device.logical.allocateMemory(
                vk::MemoryAllocateInfo(
                    reqs.size,
                    priv::getMemoryType(priv::VkEnv::device.physical, reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)
                )
            );

            priv::VkEnv::device.logical.bindBufferMemory(m_buffer, m_memory, 0U);

            priv::copyBufferToBuffer(stagingBuffer, m_buffer, memLen);

            priv::VkEnv::device.logical.destroyBuffer(stagingBuffer);
        }

        inline vk::Buffer getBuffer() const noexcept 
        {
            return m_buffer;
        }

        VertexBuffer(const VertexBuffer&) = default;

        VertexBuffer(VertexBuffer&&) = default;

        ~VertexBuffer();
    private:
        vk::Buffer m_buffer;
        vk::DeviceMemory m_memory;
    };
}