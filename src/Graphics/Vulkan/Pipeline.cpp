#include <Graphics/Vulkan/Pipeline.hpp>
#include <Exception/Exception.hpp>

#include <iostream>
#include <experimental/filesystem>

namespace iona {
    Shader::Shader(const std::string_view vertex, const std::string_view fragment) {
        {
            std::vector<char> shaderData;

            std::ifstream file(vertex.data(), std::ios::binary | std::ios::ate);

            assert(file && "no file found");

            std::size_t sz = file.tellg();

            shaderData.resize(sz);

            file.seekg(std::streampos(0));

            file.read(shaderData.data(), sz);

            file.close();

            auto smci = vk::ShaderModuleCreateInfo(
                vk::ShaderModuleCreateFlags(),
                shaderData.size(),
                reinterpret_cast<uint32_t*>(shaderData.data())
            );

            m_vertex = priv::VKInfo::device.createShaderModule(smci);
        }

        {
            std::vector<char> shaderData;

            std::ifstream file(fragment.data(), std::ios::binary | std::ios::ate);

            assert(file && "no file found");

            std::size_t sz = file.tellg();

            shaderData.resize(sz);

            file.seekg(std::streampos(0));

            file.read(shaderData.data(), sz);

            file.close();

            auto smci = vk::ShaderModuleCreateInfo(
                vk::ShaderModuleCreateFlags(),
                shaderData.size(),
                reinterpret_cast<uint32_t*>(shaderData.data())
            );

            m_fragment = priv::VKInfo::device.createShaderModule(smci);
        }

        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages { 
            vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, m_vertex, "main"),
            vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, m_fragment, "main")
        };

        auto vertexInput = vk::PipelineVertexInputStateCreateInfo(vk::PipelineVertexInputStateCreateFlags(),
            0,
            nullptr,
            0,
            nullptr
        );

        auto assembly = vk::PipelineInputAssemblyStateCreateInfo(vk::PipelineInputAssemblyStateCreateFlags(),
            vk::PrimitiveTopology::eTriangleFan,
            VK_FALSE
        );

        std::array<vk::Viewport, 1> viewports { vk::Viewport(0.f, 0.f, 800.f, 800.f, 0.f, 1.f) };
        
        std::array<vk::Rect2D, 1> scissors { vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(800, 800)) };

        auto viewport = vk::PipelineViewportStateCreateInfo(vk::PipelineViewportStateCreateFlags(),
            viewports.size(),
            viewports.data(),
            scissors.size(),
            scissors.data()
        );

        auto rasterizer = vk::PipelineRasterizationStateCreateInfo(vk::PipelineRasterizationStateCreateFlags(),
            VK_FALSE,
            VK_FALSE,
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eBack,
            vk::FrontFace::eClockwise,
            VK_FALSE,
            0.f,
            0.f,
            0.f,
            1.f
        );

        auto multisampler = vk::PipelineMultisampleStateCreateInfo(vk::PipelineMultisampleStateCreateFlags(),
            vk::SampleCountFlagBits::e1,
            VK_FALSE,
            0.f,
            0,
            VK_FALSE,
            VK_TRUE
        );

        std::array<vk::PipelineColorBlendAttachmentState, 1> attachs {
             vk::PipelineColorBlendAttachmentState().setColorWriteMask(
                 vk::ColorComponentFlagBits::eR | 
                 vk::ColorComponentFlagBits::eG | 
                 vk::ColorComponentFlagBits::eB | 
                 vk::ColorComponentFlagBits::eA
             ).setBlendEnable(VK_FALSE)
        };

        auto colorBlend = vk::PipelineColorBlendStateCreateInfo(vk::PipelineColorBlendStateCreateFlags(),
            VK_FALSE,
            vk::LogicOp::eCopy,
            attachs.size(),
            attachs.data(),
            {{ 0.f, 0.f, 0.f, 0.f }}
        );

        std::array<vk::DynamicState, 2u> dynStates {
            vk::DynamicState::eLineWidth,
            vk::DynamicState::eViewport
        };

        auto dynState = vk::PipelineDynamicStateCreateInfo(vk::PipelineDynamicStateCreateFlags(),
            dynStates.size(),
            dynStates.data()
        );

        std::array<vk::DescriptorSetLayoutBinding, 1u> bindings {
            vk::DescriptorSetLayoutBinding(1u, vk::DescriptorType::eCombinedImageSampler, 1u, vk::ShaderStageFlagBits::eFragment)
        };

        dl[0] =  priv::VKInfo::device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo(
                    vk::DescriptorSetLayoutCreateFlags(),
                    bindings.size(),
                    bindings.data()
                ));

        auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), dl.size(), dl.data());

        layout = priv::VKInfo::device.createPipelineLayout(pipelineLayoutInfo);

        auto pipelineInfo = vk::GraphicsPipelineCreateInfo(
            vk::PipelineCreateFlags(),
            shaderStages.size(),
            shaderStages.data(),
            &vertexInput,
            &assembly,
            nullptr,
            &viewport,
            &rasterizer,
            &multisampler,
            nullptr,
            &colorBlend,
            &dynState,
            layout,
            priv::VKInfo::renderPass
        );

        m_pipeline = priv::VKInfo::device.createGraphicsPipeline(vk::PipelineCache(), pipelineInfo);

        priv::VKInfo::device.destroyShaderModule(m_fragment);
        priv::VKInfo::device.destroyShaderModule(m_vertex);
    }

    void Shader::bind() {
        current = this;
    }

    Shader::~Shader() {
       
    }
    
    vk::Pipeline Shader::get() {
        return m_pipeline;
    }
}