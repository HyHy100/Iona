#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <atomic>

namespace iona::priv {
	class VKInfo {
	public:
		static inline vk::Instance instance;
		static inline vk::Device device;
		static inline vk::PhysicalDevice phyDevice;
		static inline vk::RenderPass renderPass;
		static inline vk::CommandPool commandPool;
		static inline std::vector<vk::CommandBuffer> commandBuffers;
		static inline vk::Queue graphicsQueue;
		static inline vk::DescriptorPool descPool;
	};
}