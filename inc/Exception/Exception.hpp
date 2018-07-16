#pragma once

#include <string>
#include <string_view>

namespace iona 
{
	class Exception 
	{
	public:
		Exception(const std::string_view content);

		virtual ~Exception() = default;

		virtual const std::string& what() const noexcept;
	private:
		std::string m_content;
	};
}