#pragma once

#include <vulkan/vulkan.hpp>
#include <string_view>
#include <Generic/Size.hpp>
#include <Exception/Exception.hpp>

#include <Graphics/Vulkan/Helpers/Memory.hpp>
#include <Graphics/Vulkan/VKInfo.hpp>
#include <Generic/Size.hpp>
#include <tuple>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <fstream>
#include <mutex>

namespace iona {
    class Texture {
    public:
        Texture(const std::string_view path);

        ~Texture();
    private:
        std::pair<vk::Image, vk::DeviceMemory> createImage(uSize_t size, vk::Format format, uint32_t levels, uint32_t layers);

        std::pair<vk::Buffer, vk::DeviceMemory> createStagingBuffer(const vk::Image& image);

        void uploadContentToMemory(const std::string_view path, const vk::DeviceMemory& memory, uSize_t imageSize, const stbi_uc* const data);

        vk::Image m_image;
        vk::DeviceMemory m_imageMemory;
        
        vk::Buffer m_stagingBuffer;
        vk::DeviceMemory m_stagingMemory;
        
        vk::ImageView m_imageView;

        std::mutex m_mapMutex;
    };
}