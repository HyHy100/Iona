#include <Window/Window.hpp>
#include <Graphics/Vulkan/VKInfo.hpp>
#include <Graphics/Vulkan/Helpers/QueueFamilies.hpp>
#include <Graphics/Vulkan/Helpers/Swapchain.hpp>
#include <iostream>
#include <algorithm>

namespace iona {
	Window::Window(uSize_t size, const std::string_view title) {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_pWindow = glfwCreateWindow(size.w(), size.h(), title.data(), nullptr, nullptr);

		assert(glfwCreateWindowSurface(priv::VKInfo::instance, m_pWindow, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_surface)) == static_cast<VkResult>(vk::Result::eSuccess));

		auto indices = priv::queryFamilyIndices(priv::VKInfo::instance, priv::VKInfo::phyDevice);

		assert(indices.ok());

		auto details = priv::querySwapchainSupport(priv::VKInfo::phyDevice, m_surface);

		auto formatIt = std::find_if(details.formats.begin(), details.formats.end(), [](const vk::SurfaceFormatKHR& availableFormat) {
			return (availableFormat.format == vk::Format::eB8G8R8A8Unorm) && (availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear);
		});

		if (formatIt == std::end(details.formats)) {
			formatIt = std::begin(details.formats);
		}

		auto presentModeIt = std::find_if(details.presentModes.begin(), details.presentModes.end(), [](const vk::PresentModeKHR& availableMode) {
			return availableMode == vk::PresentModeKHR::eMailbox;
		});

		if (presentModeIt == std::end(details.presentModes)) {
			presentModeIt = std::begin(details.presentModes);
		}

		m_swapChainExtent = vk::Extent2D(
				std::clamp(size.w(), details.capabilities.minImageExtent.width, details.capabilities.maxImageExtent.width),
				std::clamp(size.h(), details.capabilities.minImageExtent.height, details.capabilities.maxImageExtent.height)
			);

		std::cout << m_swapChainExtent.width << "x" << m_swapChainExtent.height << std::endl;

		auto scci = vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(),
			m_surface,
			details.capabilities.minImageCount,
			formatIt->format,
			formatIt->colorSpace,
			m_swapChainExtent,
			details.capabilities.maxImageArrayLayers,
			vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive,
			1,
			&indices.graphicsFamily,
			details.capabilities.currentTransform,
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			*presentModeIt,
			true
		);

		m_format = formatIt->format;
		m_colorSpace = formatIt->colorSpace;

		m_swapChain = priv::VKInfo::device.createSwapchainKHR(scci);

		assert(m_swapChain);

		m_swapchainImages = priv::VKInfo::device.getSwapchainImagesKHR(m_swapChain);

		assert(m_swapchainImages.size() != 0);

		std::transform(m_swapchainImages.begin(), m_swapchainImages.end(), std::back_inserter(m_swapChainImageViews), [&](const vk::Image& image) -> vk::ImageView {
			auto ivci = vk::ImageViewCreateInfo(
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
			);

			return priv::VKInfo::device.createImageView(ivci);
		});

		
		std::array<vk::AttachmentDescription, 1> colorAttachments{
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
			vk::ImageLayout::eColorAttachmentOptimal
		);

		auto subpass = vk::SubpassDescription(
			vk::SubpassDescriptionFlags(),
			vk::PipelineBindPoint::eGraphics,
			0,
			nullptr,
			1,
			&colorAttachmentRef
		);

		auto dependency = vk::SubpassDependency(
			0U,
			0U,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::AccessFlags(),
			vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
		);

		auto rpci = vk::RenderPassCreateInfo(
			vk::RenderPassCreateFlags(),
			colorAttachments.size(),
			colorAttachments.data(),
			1,
			&subpass,
			1,
			&dependency
		);

		priv::VKInfo::renderPass = priv::VKInfo::device.createRenderPass(rpci);

		std::transform(m_swapChainImageViews.begin(), m_swapChainImageViews.end(), std::back_inserter(m_frameBuffers), [&](const vk::ImageView imageView) -> vk::Framebuffer {
			auto fbci = vk::FramebufferCreateInfo(
				vk::FramebufferCreateFlags(),
				priv::VKInfo::renderPass,
				1,
				&imageView,
				m_swapChainExtent.width,
				m_swapChainExtent.height,
				1
			);

			return priv::VKInfo::device.createFramebuffer(fbci);
		});

		auto cpci = vk::CommandPoolCreateInfo(
			vk::CommandPoolCreateFlagBits::eTransient,
			indices.graphicsFamily
		);

		priv::VKInfo::commandPool = priv::VKInfo::device.createCommandPool(cpci);
		
		m_shader = Shader("Res/Shaders/vert.spv", "Res/Shaders/frag.spv");

		assert(priv::VKInfo::commandPool);

		auto cbci = vk::CommandBufferAllocateInfo(
			priv::VKInfo::commandPool,
			vk::CommandBufferLevel::ePrimary,
			m_frameBuffers.size()
		);

		priv::VKInfo::commandBuffers = priv::VKInfo::device.allocateCommandBuffers(cbci);

		std::for_each(priv::VKInfo::commandBuffers.begin(), priv::VKInfo::commandBuffers.end(), [&, i = 0](vk::CommandBuffer& commandBuffer) mutable {
			auto cbbi = vk::CommandBufferBeginInfo(
				vk::CommandBufferUsageFlagBits::eSimultaneousUse
			);
			commandBuffer.begin(cbbi);

			vk::ClearValue clearColor;
			clearColor.setColor(vk::ClearColorValue().setFloat32({ 1.f, 1.f, 0.f, 1.f }));

			auto rpbi = vk::RenderPassBeginInfo(
				priv::VKInfo::renderPass,
				m_frameBuffers[i],
				vk::Rect2D(vk::Offset2D(0, 0), m_swapChainExtent),
				1U,
				&clearColor
			);

			commandBuffer.beginRenderPass(rpbi, vk::SubpassContents::eInline);

			auto viewport = vk::Viewport(0.f, 0.f, 800.f, 800.f, 0.f, 1.f);

			commandBuffer.setViewport(0, viewport);

			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_shader.get());

			commandBuffer.draw(3U, 1U, 0U, 0U);

			commandBuffer.endRenderPass();

			commandBuffer.end();

			i++;
		});

		m_imageAvailableSemaphore = priv::VKInfo::device.createSemaphore(vk::SemaphoreCreateInfo());

		m_renderFinishedSemaphore = priv::VKInfo::device.createSemaphore(vk::SemaphoreCreateInfo());

		assert(m_imageAvailableSemaphore && m_renderFinishedSemaphore);
	}

	Window::~Window() {
		glfwDestroyWindow(m_pWindow);
	}

	bool Window::pollEvents() {
		glfwPollEvents();
		return !glfwWindowShouldClose(m_pWindow);
	}

	void Window::present() {
		vk::Result res;

		auto nextImage = priv::VKInfo::device.acquireNextImageKHR(m_swapChain, std::numeric_limits<uint16_t>::max(), m_imageAvailableSemaphore, vk::Fence());

		if (nextImage.result != vk::Result::eSuccess) {
			std::cerr << "Next image failed" << std::endl;
		}

		auto imageIndex = nextImage.value;

		std::array<vk::PipelineStageFlags, 1> waitStages = {
			vk::PipelineStageFlagBits::eColorAttachmentOutput
		};

		auto submitInfo = vk::SubmitInfo(
			1,
			&m_imageAvailableSemaphore,
			waitStages.data(),
			1,
			&priv::VKInfo::commandBuffers[imageIndex],
			1,
			&m_renderFinishedSemaphore
		);		

		priv::VKInfo::graphicsQueue.submit(submitInfo, vk::Fence());

		auto presentInfo = vk::PresentInfoKHR(
			1,
			&m_renderFinishedSemaphore,
			1,
			&m_swapChain,
			&imageIndex,
			&res
		);

		priv::VKInfo::graphicsQueue.presentKHR(presentInfo);

		priv::VKInfo::device.waitIdle();
	}
}