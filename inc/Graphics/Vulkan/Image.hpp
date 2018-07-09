#pragma once

#include <vulkan/vulkan.hpp>
#include <string_view>
#include <Generic/Size.hpp>
#include <Exception/Exception.hpp>

#include <Graphics/Vulkan/Helpers/Memory.hpp>
#include <Graphics/Vulkan/VKInfo.hpp>
#include <Generic/Size.hpp>
#include <tuple>
#include <fstream>
#include <mutex>
#include <Graphics/Vulkan/Pipeline.hpp>
#include <Graphics/Types.hpp>
#include <Graphics/Pixel.hpp>

namespace iona::priv {
    void copyBufferToBuffer(const vk::Buffer& srcBuffer, 
                                    vk::Buffer& dstBuffer, 
                                    vk::DeviceSize size);


    void copyBufferToImage(vk::Buffer src, 
                                    vk::Image dst, 
                                    SizeUint size);

    void layout2layout(const vk::Image& image, 
                                vk::Format format, 
                                vk::ImageLayout oldLayout, 
                                vk::ImageLayout newLayout);

    void uploadContentToMemory(const std::string_view path, 
                                        const vk::DeviceMemory& memory, 
                                        SizeUint imageSize, 
                                        const PixelFormatRGBA* const data);
}

namespace iona {
    class Texture {
    public:
        Texture(const std::string_view path);

        ~Texture();

        void bind(const Shader& pipeline);
    private:
        void createImage(SizeUint size, vk::Format format, uint32_t levels, uint32_t layers);

        void createStagingBuffer(const vk::Image& image);

        void createImageView();

        void createSamplers();

        vk::ImageView m_imageView;

        vk::Sampler m_sampler;

        vk::Image m_image;
        vk::DeviceMemory m_imageMemory;

        vk::Buffer m_stagingBuffer;
        vk::DeviceMemory m_stagingMemory;

        uint32_t w, h;
    };
}
