#pragma once

#include <vulkan/vulkan.hpp>

#include <Math/Matrix.hpp>
#include <Math/Types.hpp>

namespace iona {
    class Vertex 
    {
    public:
        Vertex() = default;

        Vertex(Vector4f _position) 
        : position(_position) 
        {
        }

        Vertex(const Vertex&) = default;

        Vertex(Vertex&&) = default;

        ~Vertex() = default;

        enum 
        {
            AttributeNumber = 1U,
            ElementNumber   = 1U
        };

        Vector4f position;

        static vk::VertexInputBindingDescription getBindingDescription() noexcept
        {
            return vk::VertexInputBindingDescription(
                0U,
                sizeof(Vertex),
                vk::VertexInputRate::eVertex
            );
        }

        static std::array<vk::VertexInputAttributeDescription, 1> getAttributeDescription() noexcept 
        {
            return {
                vk::VertexInputAttributeDescription(
                    0U,
                    0U,
                    vk::Format::eR32G32B32A32Sfloat,
                    offsetof(Vertex, position)
                )
            };
        }

        inline void operator=(Vertex& right) 
        {
            std::memcpy(this, &right, sizeof(*this));
        }
    };
}