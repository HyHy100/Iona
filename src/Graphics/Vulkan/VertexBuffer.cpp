#include <Graphics/Vulkan/VertexBuffer.hpp>
#include <Graphics/Vulkan/Helpers/Transfer.hpp>
#include <Graphics/Vulkan/Helpers/Memory.hpp>
#include <Graphics/Vulkan/VKInfo.hpp>
#include <Graphics/Vertex.hpp>

#include <mutex>

namespace iona 
{
    VertexBuffer::~VertexBuffer() 
    {
        if (!m_buffer)
        {
            priv::VkEnv::device.logical.destroyBuffer(m_buffer);
        }
        
        if (!m_memory)
        {
            priv::VkEnv::device.logical.freeMemory(m_memory);
        }
    }
}