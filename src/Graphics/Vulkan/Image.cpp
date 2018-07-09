#include <Graphics/Vulkan/Image.hpp>
#include <Graphics/Vulkan/Helpers/CommandBuffer.hpp>
#include <Graphics/Vulkan/Pipeline.hpp>
#include <Graphics/ImageLoader.hpp>


namespace iona::priv {
    void copyBufferToBuffer(const vk::Buffer& srcBuffer, 
                                    vk::Buffer& dstBuffer, 
                                    vk::DeviceSize size) 
    {
        auto cmdbf = priv::beginTempCommandBuffer();

        auto bfregion = vk::BufferCopy(vk::DeviceSize(0U), vk::DeviceSize(0U), size);
        cmdbf.copyBuffer(srcBuffer, dstBuffer, bfregion);

        priv::endTempCommandBuffer(cmdbf);
    }


    void copyBufferToImage(vk::Buffer src, 
                                    vk::Image dst, 
                                    SizeUint size) 
    {
        auto cmdbf = priv::beginTempCommandBuffer();

        auto region = vk::BufferImageCopy(
            vk::DeviceSize(0),
            0,
            0,
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1U),
            vk::Offset3D(0, 0, 0),
            vk::Extent3D(size.w, size.h, 1)
        );

        cmdbf.copyBufferToImage(src, dst, vk::ImageLayout::eTransferDstOptimal, region);

        priv::endTempCommandBuffer(cmdbf);
    }

    void layout2layout(const vk::Image& image, 
                                vk::Format format, 
                                vk::ImageLayout oldLayout, 
                                vk::ImageLayout newLayout) 
    {
        auto cmdbf = priv::beginTempCommandBuffer();

        auto barrier = vk::ImageMemoryBarrier(
            vk::AccessFlagBits(),
            vk::AccessFlags(),
            oldLayout,
            newLayout,
            VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED,
            image,
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0U, 1U, 0U, 1U)
        );

        cmdbf.pipelineBarrier(vk::PipelineStageFlags(), vk::PipelineStageFlags(), vk::DependencyFlags(), nullptr, nullptr, barrier);

        priv::endTempCommandBuffer(cmdbf);
    }

    void uploadContentToMemory(const std::string_view path, 
                                        const vk::DeviceMemory& memory, 
                                        SizeUint imageSize, 
                                        const PixelFormatRGBA* const data) 
    {
        static std::mutex m_mapMutex;

        m_mapMutex.lock();

        void *pMemory = priv::VKInfo::device.mapMemory(memory, 0U, VK_WHOLE_SIZE);
        assert(pMemory);

        std::memcpy(pMemory, data, imageSize.w * imageSize.h * 4);

        priv::VKInfo::device.unmapMemory(memory);

        m_mapMutex.unlock();
    }
}

namespace iona {
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

        m_image = priv::VKInfo::device.createImage(imageInfo);

        auto requirements = priv::VKInfo::device.getImageMemoryRequirements(m_image);

        auto typeIndex = priv::getMemoryType(priv::VKInfo::phyDevice, requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

        auto memoryInfo = vk::MemoryAllocateInfo(
            requirements.size,
            typeIndex
        );

        m_imageMemory = priv::VKInfo::device.allocateMemory(memoryInfo);

        priv::VKInfo::device.bindImageMemory(m_image, m_imageMemory, 0U);
    }

    void Texture::createStagingBuffer(const vk::Image& image) {
        auto requirements = priv::VKInfo::device.getImageMemoryRequirements(image);

        m_stagingBuffer = priv::VKInfo::device.createBuffer(vk::BufferCreateInfo(
            vk::BufferCreateFlags(),
            requirements.size,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::SharingMode::eExclusive
        ));

        auto reqs = priv::VKInfo::device.getBufferMemoryRequirements(m_stagingBuffer);

        m_stagingMemory = priv::VKInfo::device.allocateMemory(vk::MemoryAllocateInfo(
            reqs.size,
            0x9
        ));

        priv::VKInfo::device.bindBufferMemory(m_stagingBuffer, m_stagingMemory, 0);
    }

    void Texture::createSamplers() 
    {
        m_sampler = priv::VKInfo::device.createSampler(vk::SamplerCreateInfo(
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
        m_imageView = priv::VKInfo::device.createImageView(vk::ImageViewCreateInfo(
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
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1U, 0, 1U)
        ));
    }

    Texture::Texture(const std::string_view path) 
    {
        auto img = priv::ImageLoader::get().loadImage(path);

        createImage(img.size, vk::Format::eR8G8B8A8Unorm, 1, 1);

        createStagingBuffer(m_image);

        priv::VKInfo::device.bindBufferMemory(m_stagingBuffer, m_stagingMemory, 0U);

        priv::uploadContentToMemory(path, m_stagingMemory, img.size, img.pixels);
        priv::layout2layout(m_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        priv::copyBufferToImage(m_stagingBuffer, m_image, img.size);
        priv::layout2layout(m_image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        priv::VKInfo::device.destroyBuffer(m_stagingBuffer);
        priv::VKInfo::device.freeMemory(m_stagingMemory);

        createImageView();

        createSamplers();

        priv::ImageLoader::get().unloadImage(img);
    }

    Texture::~Texture() 
    {
    }

    void Texture::bind(const Shader& pipeline) 
    {
        auto imginfo = vk::DescriptorImageInfo(m_sampler, m_imageView, vk::ImageLayout::eShaderReadOnlyOptimal);

        auto descs = priv::VKInfo::device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(
            priv::VKInfo::descPool,
            Shader::current->dl.size(),
            Shader::current->dl.data()
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

        priv::VKInfo::device.updateDescriptorSets(wrDesc, nullptr);

        auto cb = priv::beginTempCommandBuffer();
        
        cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, Shader::current->layout, 0, descs, nullptr);
        
        priv::endTempCommandBuffer(cb);
    }
}
