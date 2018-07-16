#include <algorithm>
#include <iostream>
#include <iterator>
#include <array>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <Graphics/Vulkan/VkLoader.hpp>
#include <Graphics/Vulkan/VKInfo.hpp>
#include <Exception/Exception.hpp>
#include <Graphics/Vulkan/Helpers/QueueFamilies.hpp>

#include <Log/Logger.hpp>

#include <gsl/gsl>

namespace iona::priv 
{
	VkLoader::VkLoader() 
	{
		uint32_t extC;
		
		auto** exts = glfwGetRequiredInstanceExtensions(&extC);

		vk::ApplicationInfo appInfo(
			"Iona application",
			VK_MAKE_VERSION(1, 0, 0), 
			"Iona",
			VK_MAKE_VERSION(1, 0, 0), 
			VK_API_VERSION_1_1
		);

		std::vector<const char*> iExts {
			VK_KHR_SURFACE_EXTENSION_NAME
		};

		for (auto i = 0; i < extC; i++) 
		{
			iExts.push_back(exts[i]);
		}

		VkEnv::instance = vk::createInstance(
			vk::InstanceCreateInfo(
				vk::InstanceCreateFlags(),
				&appInfo,
				0,
				nullptr,
				iExts.size(),
				iExts.data()
			)
		);
		
		if (!VkEnv::instance) 
		{
			throw Exception("Vulkan instance create fail.");
		}

		if (auto phyDevs = VkEnv::instance.enumeratePhysicalDevices(); !phyDevs.empty())
		{
			VkEnv::device.physical = phyDevs.front();

			auto& selectedPhyDev = VkEnv::device.physical;

			std::array<const char*, 1> layerRequirements {
				"VK_LAYER_LUNARG_standard_validation"	
			};

			std::vector<const char*> deviceLayers;

			auto availableLayers = selectedPhyDev.enumerateDeviceLayerProperties();

			std::copy_if(layerRequirements.begin(), layerRequirements.end(), std::back_inserter(deviceLayers),
			[&](const char* layer) 
			{
				auto it = std::find_if(
					availableLayers.begin(), availableLayers.end(), 
					[&](const vk::LayerProperties& layerProps) 
					{
						return std::strcmp(layer, layerProps.layerName);
					}
				);

				return it == std::end(availableLayers);
			});

			auto props = selectedPhyDev.getQueueFamilyProperties();

			std::vector<vk::DeviceQueueCreateInfo> queueCreateList;
			
			std::all_of(props.begin(), props.end(), 
			[&, i = 0](auto& prop) 
			mutable {
				if (static_cast<bool>(prop.queueFlags & vk::QueueFlagBits::eGraphics)) 
				{
					float pri{ 0.f };
					
					queueCreateList.push_back(
						vk::DeviceQueueCreateInfo(
							vk::DeviceQueueCreateFlags(), 
							i, 
							1, 
							&pri
						)
					);

					return true;
				}

				i++;

				return false;
			});

			if (queueCreateList.empty())
			{
				Logger::get() << Logger::Output::Critical << "No available queue found." << std::ends;
				Logger::get().flush();
			}

			std::array<const char*, 1U> deviceExtensions {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME	
			};

			auto& logicalDevice = VkEnv::device.logical = selectedPhyDev.createDevice(
				vk::DeviceCreateInfo(
					vk::DeviceCreateFlags(),
					static_cast<uint32_t>(queueCreateList.size()),
					queueCreateList.data(),
					deviceLayers.size(),
					deviceLayers.data(),
					deviceExtensions.size(),
					deviceExtensions.data(),
					nullptr
				)
			);

			const auto queueFamilies = queryFamilyIndices(VkEnv::instance, selectedPhyDev);

			VkEnv::queues.graphics = logicalDevice.getQueue(queueFamilies.graphicsFamily, 0U);

			std::array<vk::DescriptorPoolSize, 1> poolSizes {
				vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 1U)
			};

			VkEnv::descriptors.pool = VkEnv::device.logical.createDescriptorPool(
				vk::DescriptorPoolCreateInfo(
					vk::DescriptorPoolCreateFlags(), 
					1U, 
					poolSizes.size(), 
					poolSizes.data()
				)
			);

			auto indices = queryFamilyIndices(VkEnv::instance, selectedPhyDev);

			assert(indices.ok());

			VkEnv::commands.pool = VkEnv::device.logical.createCommandPool(vk::CommandPoolCreateInfo(
				vk::CommandPoolCreateFlagBits::eTransient,
				indices.graphicsFamily
			));

			assert(VkEnv::commands.pool);

			/*
			=================================================================================
			====	Allocate one command buffer for "pre-window" operations. 			=====
			==== 	More will be created on Window setup.								=====
			=================================================================================
			*/

			VkEnv::commands.buffers = logicalDevice.allocateCommandBuffers(
				vk::CommandBufferAllocateInfo(
					VkEnv::commands.pool,
					vk::CommandBufferLevel::ePrimary,
					1U
				)
			);

			auto& cmdbuf = VkEnv::commands.buffers.front();

			cmdbuf.begin(
				vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse)
			);

			VkEnv::commands.currentBuffer = cmdbuf;
		}
		else
		{
			Logger::get() << Logger::Output::Critical << "No physical device found." << std::ends;
			Logger::get().flush();
		}
	}

	VkLoader::~VkLoader() 
	{
		VkEnv::device.logical.freeCommandBuffers(VkEnv::commands.pool, VkEnv::commands.buffers);
		VkEnv::device.logical.destroyCommandPool(VkEnv::commands.pool);
		VkEnv::device.logical.destroyDescriptorPool(VkEnv::descriptors.pool);
		VkEnv::device.logical.waitIdle();
		VkEnv::device.logical.destroy();
	}
}