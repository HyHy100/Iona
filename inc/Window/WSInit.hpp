#pragma once

#include <GLFW/glfw3.h>
#include <Exception/Exception.hpp>

#ifndef _WS_INIT_H
#define _WS_INIT_H
#endif

namespace iona::priv {
	class WSInit {
	public:
		~WSInit() = default;
	private:
		WSInit() {
			if (!glfwInit()) {
				throw Exception("GLFW init fail");
			}
			else {
				if (!glfwVulkanSupported()) {
					throw Exception("Vulkan is not supported by GLFW3");
				}
			}
		}

		friend class WSInitializerImpl;
	};

	class WSInitializerImpl {
	public:
		WSInitializerImpl() = delete;
	private:
		static inline WSInit impl;
	};
}