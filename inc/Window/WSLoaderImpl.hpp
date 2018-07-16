#pragma once

#include <Window/WSInit.hpp>

namespace iona::priv
{
    class WSLoaderImpl 
    {
	public:
		WSLoaderImpl() = delete;
	private:
		static inline WSLoader impl;
	};
}