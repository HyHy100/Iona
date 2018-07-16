#pragma once

namespace iona::priv {
	class VkLoaderImpl;

	class VkLoader {
	public:
		~VkLoader();
	private:
		VkLoader();
		
		friend class VkLoaderImpl;
	};

	class VkLoaderImpl {
	public:
		VkLoaderImpl() = delete;
	private:
		static inline VkLoader impl;
	};
}