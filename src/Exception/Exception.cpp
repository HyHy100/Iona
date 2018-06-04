#include<Exception/Exception.hpp>

namespace iona {
	Exception::Exception(const std::string_view content) : m_content(content.data()) {
	}

	std::string Exception::what() const noexcept {
		return m_content;
	}
}