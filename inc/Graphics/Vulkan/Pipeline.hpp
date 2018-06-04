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

        vk::Pipeline get();
    private:
        vk::Pipeline m_pipeline;
        vk::ShaderModule m_fragment;
        vk::ShaderModule m_vertex;
        vk::PipelineLayout layout;
    };
}