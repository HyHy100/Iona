#pragma once

#include <cstdint>

namespace iona {
    using ColorComponent = std::uint8_t;

    class PixelFormat {
    };

    class PixelFormatRGB : public PixelFormat {
    public:
        PixelFormatRGB() = default;
        
        volatile ColorComponent r, g, b;
    };

    static_assert(sizeof(PixelFormatRGB) == 3);

    class PixelFormatRGBA : public PixelFormat {
    public:
        PixelFormatRGBA() = default;

        volatile ColorComponent r, g, b, a;
    };

    static_assert(sizeof(PixelFormatRGBA) == 4);

    class PixelFormatBGR : public PixelFormat {
    public:
        PixelFormatBGR() = default;
        
        volatile ColorComponent b, g, r;
    };

    static_assert(sizeof(PixelFormatBGR) == 3);

    class PixelFormatBGRA : public PixelFormat {
    public:
        PixelFormatBGRA() = default;

        volatile ColorComponent b, g, r, a;
    };

    static_assert(sizeof(PixelFormatBGRA) == 4);

    template<
        class T = PixelFormatRGBA
    >
    struct Color : public T {
        static_assert(std::is_convertible_v<T, PixelFormat>, "Color::Error: T ins't convertible to PixelFormat.");
    };

    template<typename T = PixelFormatRGBA>
    using Pixel = struct Color<T>;
    
    namespace priv {
        enum class PixelFormats {
            eRGB, eRGBA, eBGR, eBGRA
        };
    }
}