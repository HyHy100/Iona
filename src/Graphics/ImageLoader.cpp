#include <Graphics/ImageLoader.hpp>

#if !defined(STB_IMAGE_IMPLEMENTATION)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

namespace iona::priv 
{
    stbi_uc* ImageLoader::loadByFile(const std::string_view path, int& w, int& h, int& ch)
    {
        return stbi_load(path.data(), &w, &h, &ch, 0);
    }
}
