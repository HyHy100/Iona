#pragma once

#include <string_view>
#include <sstream>
#include <fstream>
#include <chrono>
#include <ctime>

#include <Exception/FileError.hpp>

namespace iona
{
    class Logger
    {
    public:
        enum class Output 
        {
            Critical,
            Warning,
            Normal
        };

        static inline Logger& get() 
        {
            static Logger singleton;
            return singleton;
        }

        std::stringstream& operator<<(Output outType) noexcept;

        void flush();
    private:
        Logger();

        ~Logger();

        struct {
            std::stringstream critical;
            std::stringstream normal;
            std::stringstream warning;
        } m_streams;

        std::ofstream m_fileLog;
    };
}