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
                priv::VKInfo::device.destroyBuffer(m_buffer);
            }

            if (m_memory)
            {
                priv::VKInfo::device.freeMemory(m_memory);
            }

            const std::size_t memLen = vlist.size() * sizeof(Vertex);

            static std::mutex mutex;
            
            vk::Buffer stagingBuffer = priv::VKInfo::device.createBuffer(
                vk::BufferCreateInfo(
                    vk::BufferCreateFlags(),
                    memLen,
                    vk::BufferUsageFlagBits::eTransferSrc,
                    vk::SharingMode::eExclusive
                )
            );

            auto requirements = priv::VKInfo::device.getBufferMemoryRequirements(stagingBuffer);

            const vk::DeviceMemory stagingMemory = priv::getDeviceOpMemory(requirements.size);

            priv::VKInfo::device.bindBufferMemory(stagingBuffer, stagingMemory, 0U);

            mutex.lock();

            Vertex* const data = reinterpret_cast<Vertex*>(priv::VKInfo::device.mapMemory(stagingMemory, 0U, requirements.size));

            std::memcpy(data, vlist.data(), memLen);

            priv::VKInfo::device.unmapMemory(stagingMemory);

            priv::VKInfo::device.flushMappedMemoryRanges(
                vk::MappedMemoryRange(
                    stagingMemory, 
                    0U, 
                    memLen
                )
            );

            priv::VKInfo::device.invalidateMappedMemoryRanges(
                vk::MappedMemoryRange(
                    stagingMemory, 
                    0U, 
                    memLen
                )
            );

            mutex.unlock();

            m_buffer = priv::VKInfo::device.createBuffer(
                vk::BufferCreateInfo(
                    vk::BufferCreateFlags(),
                    memLen,
                    vk::BufferUsageFlagBits::eTransferDst,
                    vk::SharingMode::eExclusive                
                )
            );

            auto reqs = priv::VKInfo::device.getBufferMemoryRequirements(m_buffer);

            m_memory = priv::VKInfo::device.allocateMemory(
                vk::MemoryAllocateInfo(
                    reqs.size,
                    priv::getMemoryType(priv::VKInfo::phyDevice, reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)
                )
            );

            priv::VKInfo::device.bindBufferMemory(m_buffer, m_memory, 0U);

            priv::copyBufferToBuffer(stagingBuffer, m_buffer, memLen);

            priv::VKInfo::device.destroyBuffer(stagingBuffer);
        }

        inline vk::Buffer getBuffer() const noexcept {
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