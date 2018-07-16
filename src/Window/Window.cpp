#include <Window/Window.hpp>
#include <Graphics/Vulkan/VKInfo.hpp>
#include <Graphics/Vulkan/Helpers/QueueFamilies.hpp>
#include <Graphics/Vulkan/Helpers/Swapchain.hpp>
#include <iostream>
#include <algorithm>

namespace iona 
{
	Window::Window(SizeUint size, const std::string_view title) 
	{
		m_imageIndex.store(0U, std::memory_order_relaxed);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		
		m_pWindow = glfwCreateWindow(size.w, size.h, title.data(), nullptr, nullptr);

		assert(glfwCreateWindowSurface(
				priv::VkEnv::instance, 
				m_pWindow, 
				nullptr, 
				reinterpret_cast<VkSurfaceKHR*>(&m_surface)
			) == static_cast<VkResult>(vk::Result::eSuccess));

		auto indices = priv::queryFamilyIndices(priv::VkEnv::instance, priv::VkEnv::device.physical);

		assert(indices.ok());
		
		auto details = priv::querySwapchainSupport(priv::VkEnv::device.physical, m_surface);

		auto formatIt = std::find_if(details.formats.begin(), details.formats.end(), 
		[](const vk::SurfaceFormatKHR& availableFormat) {
			return (availableFormat.format == vk::Format::eB8G8R8A8Unorm) && (availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear);
		});

		if (formatIt == std::end(details.formats)) 
		{
			formatIt = std::begin(details.formats);
		}

		auto presentModeIt = std::find_if(details.presentModes.begin(), details.presentModes.end(), 
		[](const vk::PresentModeKHR& availableMode) {
			return availableMode == vk::PresentModeKHR::eMailbox;
		});

		if (presentModeIt == std::end(details.presentModes)) 
		{
			presentModeIt = std::begin(details.presentModes);
		}

		m_swapChainExtent = vk::Extent2D(
			std::clamp(size.w, details.capabilities.minImageExtent.width, details.capabilities.maxImageExtent.width),
			std::clamp(size.h, details.capabilities.minImageExtent.height, details.capabilities.maxImageExtent.height)
		);

		m_format = formatIt->format;
		m_colorSpace = formatIt->colorSpace;

		m_swapChain = priv::VkEnv::device.logical.createSwapchainKHR(vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(),
			m_surface,
			details.capabilities.minImageCount,
			formatIt->format,
			formatIt->colorSpace,
			m_swapChainExtent,
			details.capabilities.maxImageArrayLayers,
			vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive,
			1U,
			&indices.graphicsFamily,
			details.capabilities.currentTransform,
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			*presentModeIt,
			true
		));

		assert(m_swapChain);

		m_swapchainImages = priv::VkEnv::device.logical.getSwapchainImagesKHR(m_swapChain);

		auto cmdbufs = priv::VkEnv::device.logical.allocateCommandBuffers(
			vk::CommandBufferAllocateInfo(
				priv::VkEnv::commands.pool,
				vk::CommandBufferLevel::ePrimary,
				m_swapchainImages.size() - 1U
			)
		);

		std::move(cmdbufs.begin(), cmdbufs.end(), std::back_inserter(priv::VkEnv::commands.buffers));

		std::transform(m_swapchainImages.begin(), m_swapchainImages.end(), std::back_inserter(m_swapChainImageViews), 
		[&](const vk::Image& image) -> vk::ImageView {
			return priv::VkEnv::device.logical.createImageView(vk::ImageViewCreateInfo(
				vk::ImageViewCreateFlags(),
				image,
				vk::ImageViewType::e2D,
				m_format,
				vk::ComponentMapping(
					vk::ComponentSwizzle::eIdentity,
					vk::ComponentSwizzle::eIdentity,
					vk::ComponentSwizzle::eIdentity,
					vk::ComponentSwizzle::eIdentity
				),
				vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0,	1)
			));
		});


		std::array<vk::AttachmentDescription, 1U> colorAttachments {
			vk::AttachmentDescription(
				vk::AttachmentDescriptionFlags(),
				m_format,
				vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear,
				vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare,
				vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::ePresentSrcKHR
			)
		};

		auto colorAttachmentRef = vk::AttachmentReference(
			0U,
			vk::ImageLayout::eShaderReadOnlyOptimal
		);

		std::array<vk::SubpassDescription, 1U> subpasses {
			vk::SubpassDescription(
				vk::SubpassDescriptionFlags(),
				vk::PipelineBindPoint::eGraphics,
				0,
				nullptr,
				1,
				&colorAttachmentRef
			)
		};

		std::array<vk::SubpassDependency, 1U> dependencies {
			vk::SubpassDependency(
				0U,
				0U,
				vk::PipelineStageFlagBits::eColorAttachmentOutput,
				vk::PipelineStageFlagBits::eColorAttachmentOutput,
				vk::AccessFlags(),
				vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
			)
		};

		priv::VkEnv::renderPass = priv::VkEnv::device.logical.createRenderPass(vk::RenderPassCreateInfo(
			vk::RenderPassCreateFlags(),
			colorAttachments.size(),
			colorAttachments.data(),
			subpasses.size(),
			subpasses.data(),
			dependencies.size(),
			dependencies.data()
		));

		std::transform(m_swapChainImageViews.begin(), m_swapChainImageViews.end(), std::back_inserter(m_frameBuffers), 
		[&](const vk::ImageView imageView) -> vk::Framebuffer {
			return priv::VkEnv::device.logical.createFramebuffer(vk::FramebufferCreateInfo(
				vk::FramebufferCreateFlags(),
				priv::VkEnv::renderPass,
				1,
				&imageView,
				m_swapChainExtent.width,
				m_swapChainExtent.height,
				1
			));
		});

		m_shader = Shader("Res/Shaders/vert.spv", "Res/Shaders/frag.spv");

		m_shader.bind();

		m_imageAvailableSemaphore = priv::VkEnv::device.logical.createSemaphore(vk::SemaphoreCreateInfo());

		m_renderFinishedSemaphore = priv::VkEnv::device.logical.createSemaphore(vk::SemaphoreCreateInfo());

		auto& cmdbuf = priv::VkEnv::commands.currentBuffer;

		vk::ClearValue clearColor;
		
		clearColor.setColor(
			vk::ClearColorValue().setFloat32(
				{ 
					0.f, 
					0.f, 
					1.f, 
					1.f 
				}
			)
		);
		
		cmdbuf.beginRenderPass(
			vk::RenderPassBeginInfo(
				priv::VkEnv::renderPass,
				m_frameBuffers.at(m_imageIndex.load(std::memory_order::memory_order_relaxed)),
				vk::Rect2D(vk::Offset2D(0, 0), m_swapChainExtent),
				1U,
				&clearColor
			), 
		vk::SubpassContents::eInline);

		
		auto viewport = vk::Viewport(0.f, 0.f, m_swapChainExtent.width, m_swapChainExtent.height, 0.f, 1.f);

		cmdbuf.setViewport(0, viewport);
	}

	Window::~Window() 
	{
		priv::VkEnv::device.logical.waitIdle();

		glfwDestroyWindow(m_pWindow);
		
		priv::VkEnv::device.logical.destroySemaphore(m_imageAvailableSemaphore);
		priv::VkEnv::device.logical.destroySemaphore(m_renderFinishedSemaphore);
		
		std::vector<vk::CommandBuffer> toFreeCommandBuffers;

		std::copy(priv::VkEnv::commands.buffers.begin() + 1, priv::VkEnv::commands.buffers.end(), std::back_inserter(toFreeCommandBuffers));

		priv::VkEnv::device.logical.freeCommandBuffers(priv::VkEnv::commands.pool, toFreeCommandBuffers);

		priv::VkEnv::commands.buffers.resize(1U);

		
		std::for_each(m_swapChainImageViews.begin(), m_swapChainImageViews.end(), 
		[](auto& view) {
			priv::VkEnv::device.logical.destroyImageView(view);
		});

		std::for_each(m_swapchainImages.begin(), m_swapchainImages.end(), 
		[](auto& view) {
			priv::VkEnv::device.logical.destroyImage(view);
		});
	}

	bool Window::pollEvents() 
	{
		glfwPollEvents();
		return !glfwWindowShouldClose(m_pWindow);
	}

	void Window::present() 
	{
		auto nextImage = priv::VkEnv::device.logical.acquireNextImageKHR(
							m_swapChain, 
							std::numeric_limits<std::size_t>::max(), 
							m_imageAvailableSemaphore,
							vk::Fence()
						);

		auto& cmdbuf = priv::VkEnv::commands.currentBuffer;
		
		cmdbuf.endRenderPass();

		cmdbuf.end();

		if (nextImage.result != vk::Result::eSuccess) 
		{
			std::cerr << "Next Image Error" << std::endl;
		}

		auto imageIndex = m_imageIndex.exchange(
			nextImage.value,
			std::memory_order::memory_order_relaxed
		);

		std::array<vk::PipelineStageFlags, 1> waitStages {
			vk::PipelineStageFlagBits::eColorAttachmentOutput
		};

		auto submitInfo = vk::SubmitInfo(
			1U,
			&m_imageAvailableSemaphore,
			waitStages.data(),
			1U,
			&priv::VkEnv::commands.buffers.at(imageIndex),
			1U,
			&m_renderFinishedSemaphore
		);		

		priv::VkEnv::queues.graphics.submit(submitInfo, vk::Fence());

		vk::Result res;

		auto presentInfo = vk::PresentInfoKHR(
			1U,
			&m_renderFinishedSemaphore,
			1U,
			&m_swapChain,
			&imageIndex,
			&res
		);

		priv::VkEnv::queues.graphics.presentKHR(presentInfo);

		if (nextImage.value < imageIndex)
		{
			priv::VkEnv::device.logical.freeCommandBuffers(priv::VkEnv::commands.pool, priv::VkEnv::commands.buffers);

			priv::VkEnv::commands.buffers = priv::VkEnv::device.logical.allocateCommandBuffers(
				vk::CommandBufferAllocateInfo(
					priv::VkEnv::commands.pool,
					vk::CommandBufferLevel::ePrimary,
					m_swapchainImages.size()
				)
			);
		}

		cmdbuf = priv::VkEnv::commands.buffers.at(nextImage.value);

		cmdbuf.begin(
			vk::CommandBufferBeginInfo(
				vk::CommandBufferUsageFlagBits::eSimultaneousUse
			)
		);

		vk::ClearValue clearColor;
		
		clearColor.setColor(
			vk::ClearColorValue().setFloat32({ 
				0.f, 
				0.f, 
				1.f, 
				1.f 
			})
		);
		
		cmdbuf.beginRenderPass(
			vk::RenderPassBeginInfo(
				priv::VkEnv::renderPass,
				m_frameBuffers.at(nextImage.value),
				vk::Rect2D(vk::Offset2D(0, 0), m_swapChainExtent),
				1U,
				&clearColor
			), 
			vk::SubpassContents::eInline
		);

		
		auto viewport = vk::Viewport(0.f, 0.f, m_swapChainExtent.width, m_swapChainExtent.height, 0.f, 1.f);

		cmdbuf.setViewport(0, viewport);

		priv::VkEnv::commands.currentBuffer = cmdbuf;

		m_shader.bind();
	}
}
