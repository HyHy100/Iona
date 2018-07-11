#pragma once

#include <vector>
#include <string_view>

#include <Graphics/Pixel.hpp>
#include <Generic/Size.hpp>

#if !defined(STB_IMAGE_IMPLEMENTATION)
#include <stb_image.h>
#endif

namespace iona::priv {
    struct ImageBufferInfo 
    {
        unsigned width, height;
        priv::PixelFormats format;
        void* data;
    };

    template<
        typename T
    >
    struct ImageInfo 
    {
        SizeUint size;
        uint32_t channels;
        std::add_pointer_t<T> pixels; 
    };

    class ImageLoader 
    {
    public:
        static ImageLoader& get() 
        {
            static ImageLoader singleton;
            return singleton;
        }

        template<
            class T = PixelFormatRGBA
        >
        ImageInfo<T> loadImage(const std::string_view path) 
        {
            static_assert(std::is_convertible_v<T, PixelFormat>, "T ins't convertible to PixelFormat.");
            
            // TODO

            static_assert(std::is_same_v<T, PixelFormatRGBA>, "Only PixelFormatRGBA supported yet.");
            
            int w, h, ch;
            stbi_uc* data = loadByFile(path.data(), w, h, ch);

            ImageInfo<T> info = {
                .size = { static_cast<unsigned>(w), static_cast<unsigned>(h) },
                .channels = static_cast<unsigned>(ch),
                .pixels = reinterpret_cast<std::add_pointer_t<T>>(data)
            };
            
            return info;
        }

        template<
            class T = PixelFormatRGBA
        >
        void unloadImage(ImageInfo<T> image) 
        {
            stbi_image_free(reinterpret_cast<void*>(image.pixels));
        }
    private:
        stbi_uc* loadByFile(const std::string_view path, int& w, int& h, int& ch);
    };
}