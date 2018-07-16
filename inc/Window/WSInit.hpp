#pragma once

#include <GLFW/glfw3.h>
#include <Exception/Exception.hpp>

namespace iona::priv 
{
	class WSLoader 
	{
	public:
		~WSLoader() = default;
	private:
		WSLoader()
		{
			if (!glfwInit())
			{
				throw Exception("GLFW init fail.");
			}
		}

		friend class WSLoaderImpl;
	};
}