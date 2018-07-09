#include <Graphics/Vulkan/RNInit.hpp>
#include <Graphics/Vulkan/VKInfo.hpp>
#include <Exception/Exception.hpp>
#define GLFW_INCLUDE_VULKAN
#include <Graphics/Vulkan/Helpers/QueueFamilies.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <array>

namespace iona::priv {
	RNInit::RNInit() {
		uint32_t extC;
		
		auto** exts = glfwGetRequiredInstanceExtensions(&extC);

		vk::ApplicationInfo appInfo(
			"IonaApp", 
			VK_MAKE_VERSION(1, 0, 0), 
			"Iona", 
			VK_MAKE_VERSION(1, 0, 0), 
			VK_API_VERSION_1_0
		);

		std::vector<const char*> iExts {
			VK_KHR_SURFACE_EXTENSION_NAME
		};

		for (auto i = 0; i < extC; i++) {
			iExts.push_back(exts[i]);
		}

		VKInfo::instance = vk::createInstance(
			vk::InstanceCreateInfo(
				vk::InstanceCreateFlags(),
				&appInfo,
				0,
				nullptr,
				iExts.size(),
				iExts.data()
			)
		);
		
		if (!VKInfo::instance) {
			throw Exception("Vulkan instance create fail");
		}

		auto phyDevs = VKInfo::instance.enumeratePhysicalDevices();
		
		if (phyDevs.empty()) {
			throw Exception("No graphic device found");
		}

		std::vector<vk::DeviceQueueCreateInfo> qCreateInfos;

		auto& dev = phyDevs[0];
		VKInfo::phyDevice = dev;

		std::array<const char*, 1> requiredLayers {
			"VK_LAYER_LUNARG_standard_validation"
		};

		/*std::vector<const char*> dLayers;
		dLayers.swap(requiredLayers);*/

		/*{
			auto availableLayers = dev.enumerateDeviceLayerProperties();

			std::copy_if(requiredLayers.begin(), requiredLayers.end(), std::back_inserter(dLayers), [&](const char* layer) {
				return std::find_if(availableLayers.begin(), availableLayers.end(), [&](const vk::LayerProperties& layerProps) {
					return std::strcmp(layer, layerProps.layerName);
				}) == std::end(availableLayers);
			});
		}*/

		auto props = dev.getQueueFamilyProperties();
		std::all_of(props.begin(), props.end(), [&, i = 0](auto& prop) mutable {
			if (static_cast<bool>(prop.queueFlags & vk::QueueFlagBits::eGraphics)) {
				float pri{ 0.f };
				qCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), i, 1, &pri));
				return true;
			}
			i++;
			return false;
		});

		if (qCreateInfos.empty()) {
			throw Exception("no graphic queue found");
		}

		std::array<const char*, 1> dExts = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VKInfo::device = dev.createDevice(vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(),
			static_cast<uint32_t>(qCreateInfos.size()),
			qCreateInfos.data(),
			requiredLayers.size(),
			requiredLayers.data(),
			dExts.size(),
			dExts.data(),
			nullptr
		));
		
		auto queueFamilies = queryFamilyIndices(VKInfo::instance, VKInfo::phyDevice);

		VKInfo::graphicsQueue = VKInfo::device.getQueue(queueFamilies.graphicsFamily, 0);

		 std::array<vk::DescriptorPoolSize, 1> poolSizes {
            vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 1U)
        };

		VKInfo::descPool = VKInfo::device.createDescriptorPool(vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlags(), 1u, poolSizes.size(), poolSizes.data()));
	}
}