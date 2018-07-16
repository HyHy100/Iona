#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace iona::priv {
	class VkEnv
	{
	public:
		static inline struct 
		{
			vk::Device logical;
			vk::PhysicalDevice physical;
		} device;

		static inline struct
		{
			vk::Queue graphics;
		} queues;

		static inline struct
		{
			vk::CommandPool pool;
			std::vector<vk::CommandBuffer> buffers;
			vk::CommandBuffer currentBuffer;
		} commands;

		static inline struct
		{
			vk::DescriptorPool pool;
		} descriptors;
		
		static inline vk::RenderPass renderPass;

		static inline vk::Instance instance;
	};
}