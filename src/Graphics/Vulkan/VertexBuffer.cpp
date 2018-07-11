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
            priv::VKInfo::device.destroyBuffer(m_buffer);
        }
        
        if (!m_memory)
        {
            priv::VKInfo::device.freeMemory(m_memory);
        }
    }
}