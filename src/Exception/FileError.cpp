#include <Exception/FileError.hpp>

namespace iona
{
     FileOpenException::FileOpenException(const std::string_view content) : Exception(content)
    {
    }
}