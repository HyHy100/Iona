#pragma once

namespace iona::priv {
	class RNInitImpl;

	class RNInit {
	public:
		~RNInit() = default;
	private:
		RNInit();
		
		friend class RNInitImpl;
	};

	class RNInitImpl {
	public:
		RNInitImpl() = delete;
	private:
		static inline RNInit impl;
	};
}