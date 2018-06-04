#include <Graphics/Vulkan/Image.hpp>

namespace iona {
    std::pair<vk::Image, vk::DeviceMemory> Texture::createImage(uSize_t size, vk::Format format, uint32_t levels, uint32_t layers) {
        vk::Image image;

        auto imageInfo = vk::ImageCreateInfo(
            vk::ImageCreateFlags(),
            vk::ImageType::e2D,
            format,
            vk::Extent3D(size.w(), size.h(), 1),
            levels,
            layers,
            vk::SampleCountFlagBits::e1,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
            vk::SharingMode::eExclusive
        );

        image = priv::VKInfo::device.createImage(imageInfo);

        auto requirements = priv::VKInfo::device.getImageMemoryRequirements(image);

        auto typeIndex = priv::getMemoryType(priv::VKInfo::phyDevice, vk::MemoryPropertyFlagBits::eDeviceLocal);

        auto memoryInfo = vk::MemoryAllocateInfo(
            requirements.size,
            typeIndex
        );

        auto memory = priv::VKInfo::device.allocateMemory(memoryInfo);
        
        priv::VKInfo::device.bindImageMemory(image, memory, 0U);

        return { image, memory };
    }

    std::pair<vk::Buffer, vk::DeviceMemory> Texture::createStagingBuffer(const vk::Image& image) {
        vk::Buffer buffer;

        auto requirements = priv::VKInfo::device.getImageMemoryRequirements(image);

        auto typeIndex = priv::getMemoryType(priv::VKInfo::phyDevice, vk::MemoryPropertyFlagBits::eDeviceLocal);

        auto stagingMemoryInfo = vk::MemoryAllocateInfo(
            requirements.size,
            typeIndex
        );

        auto memory = priv::VKInfo::device.allocateMemory(
            stagingMemoryInfo
        );

        auto bufferInfo = vk::BufferCreateInfo(
            vk::BufferCreateFlags(),
            requirements.size,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::SharingMode::eExclusive
        );

        buffer = priv::VKInfo::device.createBuffer(bufferInfo);

        priv::VKInfo::device.bindBufferMemory(buffer, m_stagingMemory, 0U);

        return { buffer, memory };
    }
    
    void Texture::uploadContentToMemory(const std::string_view path, const vk::DeviceMemory& memory, uSize_t imageSize, const stbi_uc* const data) {
        auto bz = priv::VKInfo::device.getMemoryCommitment(memory);
        assert(bz < sizeof(data));

        m_mapMutex.lock();
        
        void *pMemory = priv::VKInfo::device.mapMemory(memory, 0U, VK_WHOLE_SIZE);

        std::memcpy(pMemory, data, sizeof(data));

        priv::VKInfo::device.unmapMemory(memory);
        priv::VKInfo::device.flushMappedMemoryRanges(vk::MappedMemoryRange(memory, 0U, VK_WHOLE_SIZE));
        priv::VKInfo::device.invalidateMappedMemoryRanges(vk::MappedMemoryRange(memory, 0U, VK_WHOLE_SIZE));

        m_mapMutex.unlock();
    }

    Texture::Texture(const std::string_view path) {
        int w, h, channels;
        stbi_uc *data = stbi_load(path.data(), &w, &h, &channels, 0);

        std::tie(m_image, m_imageMemory) = createImage(uSize_t(static_cast<unsigned int>(w), static_cast<unsigned int>(h)), vk::Format::eR8G8B8A8Unorm, 1, 1);

        std::tie(m_stagingBuffer, m_stagingMemory) = createStagingBuffer(m_image);

        uploadContentToMemory(path, m_stagingMemory, uSize_t(w, h), data);

        stbi_image_free(data);
    }
}