#pragma once

#include <string_view>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <Generic/Size.hpp>
#include <Graphics/Vulkan/VKInfo.hpp>

#include <Graphics/Vulkan/Pipeline.hpp>
#include <Graphics/Vulkan/Image.hpp>

#include "WSInit.hpp"

namespace iona {
	class Window {
	public:
		Window() = delete;

		Window(SizeUint size, const std::string_view title);

		Window(const Window&) = delete;

		~Window();

		void begin();

		void end();

		bool pollEvents();

		void present();
	private:
		void createSurface();

		void createSwapchain();
		
		vk::SwapchainKHR m_swapChain;
		vk::SurfaceKHR m_surface;
		std::vector<vk::Image> m_swapchainImages;
		std::vector<vk::ImageView> m_swapChainImageViews;
		std::vector<vk::Framebuffer> m_frameBuffers;
		vk::Extent2D m_swapChainExtent;
		vk::Format m_format;
		vk::ColorSpaceKHR m_colorSpace;

		vk::Semaphore m_imageAvailableSemaphore;
		vk::Semaphore m_renderFinishedSemaphore;
		
		Shader m_shader;

		GLFWwindow *m_pWindow;
	};
}