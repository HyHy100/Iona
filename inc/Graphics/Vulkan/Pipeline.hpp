#pragma once

#include <vulkan/vulkan.hpp>
#include <Graphics/Vulkan/VKInfo.hpp>

#include <fstream>
#include <string>
#include <string_view>
#include <iterator>
#include <array>
#include <algorithm>
#include <iostream>

namespace iona 
{
    class Shader
    {
    public:
        Shader() = default;

        Shader(const std::string_view vertex, const std::string_view fragment);

        ~Shader();

        void bind();

        static Shader& current()
        {
            if (!m_current)
            {
                std::cerr << "Shader::current: Current shader pointer is null." << std::endl;
            }

            return *m_current;
        }
    private:
        friend class Texture;

        inline vk::Pipeline& getPipeline() noexcept 
        {
            static vk::Pipeline vpipeline;
            return vpipeline;
        }

        inline vk::PipelineLayout& getLayout() noexcept 
        {
            static vk::PipelineLayout vlayout;
            return vlayout;
        }

        inline std::array<vk::DescriptorSetLayout, 1>& getDescriptors() noexcept
        {
            static std::array<vk::DescriptorSetLayout, 1> vdescriptors;
            return vdescriptors;
        }

        static inline Shader* m_current{ nullptr };
    };
}