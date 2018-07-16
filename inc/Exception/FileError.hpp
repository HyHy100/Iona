#pragma once

#include <Exception/Exception.hpp>

namespace iona 
{
    class FileOpenException : public Exception
    {
    public:
        FileOpenException() = default;

        FileOpenException(const std::string_view content);

        ~FileOpenException() = default;
    };
}