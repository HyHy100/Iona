#include <Graphics/Vulkan/Image.hpp>
#include <Graphics/Vulkan/Helpers/CommandBuffer.hpp>
#include <Graphics/Vulkan/Pipeline.hpp>
#include <Graphics/ImageLoader.hpp>


namespace iona::priv 
{
    void uploadContentToMemory(const std::string_view path, 
                                        const vk::DeviceMemory& memory, 
                                        SizeUint imageSize, 
                                        const PixelFormatRGBA* const data) 
    {
        static std::mutex m_mapMutex;

        m_mapMutex.lock();

        void *pMemory = priv::VkEnv::device.logical.mapMemory(memory, 0U, VK_WHOLE_SIZE);
        assert(pMemory);

        std::memcpy(pMemory, data, imageSize.w * imageSize.h * 4);

        priv::VkEnv::device.logical.unmapMemory(memory);

        m_mapMutex.unlock();
    }
}

namespace iona 
{
    void Texture::createImage(SizeUint size,
                         vk::Format format,
                         uint32_t levels,
                         uint32_t layers) 
    {
        auto imageInfo = vk::ImageCreateInfo(
            vk::ImageCreateFlags(),
            vk::ImageType::e2D,
            format,
            vk::Extent3D(size.w, size.h, 1),
            levels,
            layers,
            vk::SampleCountFlagBits::e1,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
            vk::SharingMode::eExclusive
        );

        m_image = priv::VkEnv::device.logical.createImage(imageInfo);

        auto requirements = priv::VkEnv::device.logical.getImageMemoryRequirements(m_image);

        auto typeIndex = priv::getMemoryType(priv::VkEnv::device.physical, requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

        auto memoryInfo = vk::MemoryAllocateInfo(
            requirements.size,
            typeIndex
        );

        m_imageMemory = priv::VkEnv::device.logical.allocateMemory(memoryInfo);

        priv::VkEnv::device.logical.bindImageMemory(m_image, m_imageMemory, 0U);
    }

    void Texture::createStagingBuffer(const vk::Image& image) {
        auto requirements = priv::VkEnv::device.logical.getImageMemoryRequirements(image);

        m_stagingBuffer = priv::VkEnv::device.logical.createBuffer(vk::BufferCreateInfo(
            vk::BufferCreateFlags(),
            requirements.size,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::SharingMode::eExclusive
        ));

        auto reqs = priv::VkEnv::device.logical.getBufferMemoryRequirements(m_stagingBuffer);

        m_stagingMemory = priv::VkEnv::device.logical.allocateMemory(vk::MemoryAllocateInfo(
            reqs.size,
            priv::getMemoryType(priv::VkEnv::device.physical, reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible)
        ));

        priv::VkEnv::device.logical.bindBufferMemory(m_stagingBuffer, m_stagingMemory, 0);
    }

    void Texture::createSamplers() 
    {
        m_sampler = priv::VkEnv::device.logical.createSampler(vk::SamplerCreateInfo(
            vk::SamplerCreateFlags(),
            vk::Filter::eNearest,
            vk::Filter::eNearest,
            vk::SamplerMipmapMode::eNearest,
            vk::SamplerAddressMode::eClampToBorder,
            vk::SamplerAddressMode::eClampToBorder,
            vk::SamplerAddressMode::eClampToBorder,
            0.f,
            VK_FALSE,
            0.f,
            VK_FALSE,
            vk::CompareOp::eAlways
        ));
    }

    void Texture::createImageView() 
    {
        m_imageView = priv::VkEnv::device.logical.createImageView(vk::ImageViewCreateInfo(
            vk::ImageViewCreateFlags(),
            m_image,
            vk::ImageViewType::e2D,
            vk::Format::eR8G8B8A8Unorm,
            vk::ComponentMapping(
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity,
                vk::ComponentSwizzle::eIdentity
            ),
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0U, 1U, 0U, 1U)
        ));
    }

    Texture::Texture(const std::string_view path) 
    {
        auto img = priv::ImageLoader::get().loadImage(path);

        createImage(img.size, vk::Format::eR8G8B8A8Unorm, 1U, 1U);

        createStagingBuffer(m_image);

        priv::VkEnv::device.logical.bindBufferMemory(m_stagingBuffer, m_stagingMemory, 0U);

        priv::uploadContentToMemory(path, m_stagingMemory, img.size, img.pixels);
        priv::layout2layout(m_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        priv::copyBufferToImage(m_stagingBuffer, m_image, img.size);
        priv::layout2layout(m_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        priv::VkEnv::device.logical.destroyBuffer(m_stagingBuffer);
        priv::VkEnv::device.logical.freeMemory(m_stagingMemory);

        createImageView();

        createSamplers();

        priv::ImageLoader::get().unloadImage(img);
    }

    Texture::~Texture() 
    {
        priv::VkEnv::device.logical.destroyImageView(m_imageView);
        priv::VkEnv::device.logical.destroyImage(m_image);
        priv::VkEnv::device.logical.destroySampler(m_sampler);
        priv::VkEnv::device.logical.freeMemory(m_imageMemory);
    }

    void Texture::bind() 
    {
        auto imginfo = vk::DescriptorImageInfo(m_sampler, m_imageView, vk::ImageLayout::eShaderReadOnlyOptimal);

        auto descs = priv::VkEnv::device.logical.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(
            priv::VkEnv::descriptors.pool,
            Shader::current().getDescriptors().size(),
            Shader::current().getDescriptors().data()
        ));

        auto wrDesc = vk::WriteDescriptorSet(
            descs.front(),
            1U,
            0U,
            1U,
            vk::DescriptorType::eCombinedImageSampler,
            &imginfo,
            nullptr,
            nullptr
        );

        priv::VkEnv::device.logical.updateDescriptorSets(wrDesc, nullptr);  

        auto& cb = priv::VkEnv::commands.currentBuffer;
        
        cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, Shader::current().getLayout(), 0, descs, nullptr);

        priv::VkEnv::device.logical.freeDescriptorSets(priv::VkEnv::descriptors.pool, descs);
    }
}
