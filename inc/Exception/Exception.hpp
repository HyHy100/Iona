#pragma once

#include <string>
#include <string_view>

namespace iona {
	class Exception {
	public:
		Exception(const std::string_view content);

		std::string what() const noexcept;
	private:
		std::string m_content;
	};
}