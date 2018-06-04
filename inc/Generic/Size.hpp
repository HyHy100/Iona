#pragma once

#include <array>
#include <tuple>

namespace iona {
	template<typename T>
	class Size {
	public:
		Size() : m_w(static_cast<T>(0)), m_h(static_cast<T>(0)) {
		}

		explicit Size(T w, T h) : m_w(w), m_h(h) {
		}

		inline T w() const noexcept {
			return m_w;
		}

		inline T h() const noexcept {
			return m_h;
		}
	private:
		T m_w, m_h;
	};

	using iSize_t = Size<int>;
	using fSize_t = Size<float>;
	using uSize_t = Size<unsigned>;
	using dSize_t = Size<double>;
}