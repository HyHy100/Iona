#pragma once

#include <array>
#include <tuple>

namespace iona {
	template<typename T>
	class Size {
	public:
		Size() : 
			w(static_cast<T>(0)), h(static_cast<T>(0)) 
		{
		}

		Size(T _w, T _h) :
			w(_w), h(_h) 
		{
		}

		T w, h;
	};

	using SizeInt  	 = Size<int>;
	using SizeFloat  = Size<float>;
	using SizeUint 	 = Size<uint>;
	using SizeDouble = Size<double>;
}