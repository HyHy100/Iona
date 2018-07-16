#include <Graphics/Vulkan/Pipeline.hpp>
#include <Graphics/Vertex.hpp>
#include <Graphics/Vulkan/Helpers/CommandBuffer.hpp>

#include <Exception/FileError.hpp>

#include <Log/Logger.hpp>

#include <iostream>
#include <experimental/filesystem>

namespace iona 
{
    Shader::Shader(const std::string_view vertex, const std::string_view fragment) 
    {
        vk::ShaderModule fragmentModule, vertexModule;
        
        if (std::ifstream file(vertex.data(), std::ios::binary | std::ios::ate); file)
        {
            std::vector<char> shaderData;

            std::size_t sz = file.tellg();

            shaderData.resize(sz);

            file.seekg(std::streampos(0));

            file.read(shaderData.data(), sz);

            vertexModule = priv::VkEnv::device.logical.createShaderModule(vk::ShaderModuleCreateInfo(
                vk::ShaderModuleCreateFlags(),
                shaderData.size(),
                reinterpret_cast<uint32_t*>(shaderData.data())
            ));

            file.close();
        }
        else 
        {
            Logger::get() << Logger::Output::Critical << "Vulkan::Shader::Vertex::Error: \"" << vertex.data() << "\" Fail on open vertex shader." << std::ends;
            Logger::get().flush();

            throw FileOpenException("Vulkan::Shader::Vertex::Error: Fail on open vertex shader.");
        }

        if (std::ifstream file(fragment.data(), std::ios::binary | std::ios::ate); file)
        {
            std::vector<char> shaderData;

            std::size_t sz = file.tellg();

            shaderData.resize(sz);

            file.seekg(std::streampos(0));

            file.read(shaderData.data(), sz);

            fragmentModule = priv::VkEnv::device.logical.createShaderModule(vk::ShaderModuleCreateInfo(
                vk::ShaderModuleCreateFlags(),
                shaderData.size(),
                reinterpret_cast<uint32_t*>(shaderData.data())
            ));

            file.close();
        }
        else
        {
            Logger::get() << Logger::Output::Critical << "Vulkan::Shader::Fragment::Error: \"" << vertex.data() << "\" Fail on open fragment shader." << std::ends;
            Logger::get().flush();

            throw FileOpenException("Vulkan::Shader::Fragment::Error: Fail on open fragment shader.");
        }

        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages {
            vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, vertexModule, "main"),
            vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, fragmentModule, "main")
        };

        auto bDescription = Vertex::getBindingDescription();
        auto attrDescription = Vertex::getAttributeDescription();

        auto vertexInput = vk::PipelineVertexInputStateCreateInfo(vk::PipelineVertexInputStateCreateFlags(),
            1U,
            &bDescription,
            Vertex::AttributeNumber,
            attrDescription.data()
        );

        auto assembly = vk::PipelineInputAssemblyStateCreateInfo(vk::PipelineInputAssemblyStateCreateFlags(),
            vk::PrimitiveTopology::eTriangleFan,
            VK_FALSE
        );

        std::array<vk::Viewport, 1> viewports {
            vk::Viewport(0.f, 0.f, 800.f, 800.f, 0.f, 1.f) 
        };
        
        std::array<vk::Rect2D, 1> scissors { 
            vk::Rect2D(
                vk::Offset2D(0, 0), 
                vk::Extent2D(800, 800)
            ) 
        };

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
            {{ 
                0.f, 
                0.f, 
                0.f, 
                0.f 
            }}
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
            vk::DescriptorSetLayoutBinding( 
                1u,
                vk::DescriptorType::eCombinedImageSampler, 
                1u, 
                vk::ShaderStageFlagBits::eFragment 
            )
        };

        auto& descriptors = getDescriptors();

        descriptors.at(0) =  priv::VkEnv::device.logical.createDescriptorSetLayout(
                        vk::DescriptorSetLayoutCreateInfo(
                            vk::DescriptorSetLayoutCreateFlags(),
                            bindings.size(),
                            bindings.data()
                        )
                    );

        auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo(
            vk::PipelineLayoutCreateFlags(), 
            descriptors.size(), 
            descriptors.data()
        );

        auto& layout = getLayout();

        layout = priv::VkEnv::device.logical.createPipelineLayout(pipelineLayoutInfo);

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
            getLayout(),
            priv::VkEnv::renderPass
        );

        auto& pipe = getPipeline();

        pipe = priv::VkEnv::device.logical.createGraphicsPipeline(vk::PipelineCache(), pipelineInfo);

        priv::VkEnv::device.logical.destroyShaderModule(fragmentModule);

        priv::VkEnv::device.logical.destroyShaderModule(vertexModule);
    }

    void Shader::bind() 
    {
        priv::VkEnv::commands.currentBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, getPipeline());

        m_current = this;
    }

    Shader::~Shader() 
    {
        priv::VkEnv::device.logical.destroyPipeline(getPipeline());
    }
}