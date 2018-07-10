#pragma once

#include <vulkan/vulkan.hpp>
#include <Graphics/Vulkan/VKInfo.hpp>

#include <fstream>
#include <string>
#include <string_view>
#include <iterator>
#include <array>
#include <algorithm>

namespace iona {
    class Shader {
    public:
        Shader() = default;

        Shader(const std::string_view vertex, const std::string_view fragment);

        ~Shader();

        void bind();

        static inline Shader* current = nullptr;
    private:
        friend class Texture;

        inline vk::Pipeline& pipeline() noexcept 
        {
            return m_pipeline;
        }

        inline vk::PipelineLayout& layout() noexcept 
        {
            return m_layout;
        }

        inline std::array<vk::DescriptorSetLayout, 1>& descriptors() noexcept
        {
            return dl;
        }

        std::array<vk::DescriptorSetLayout, 1> dl;

        vk::Pipeline m_pipeline;

        vk::PipelineLayout m_layout;
    };
}