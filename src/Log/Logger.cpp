#include <Log/Logger.hpp>

namespace iona
{
    std::stringstream& Logger::operator<<(Output outType) noexcept
    {
        static std::size_t counter = 0U;

        switch (outType)
        {
            case Output::Critical:
                return m_streams.critical;
                break;
            case Output::Normal:
                return m_streams.normal;
                break;
            case Output::Warning:
                return m_streams.warning;
                break;
        }

        if (++counter == 5U)
        {
            flush();
        }
    }

    void Logger::flush()
    {
        auto err = m_streams.critical.str();

        auto& fileLog = m_fileLog;
        const auto sysClock = std::chrono::system_clock::now();
        const auto date = std::chrono::system_clock::to_time_t(sysClock);

        m_fileLog << "[" << date << "]" << std::ends;

        m_fileLog << "[Output]" << std::ends;
        m_fileLog << m_streams.normal.str() << std::ends;

        m_fileLog << "[Errors]" << std::ends;
        m_fileLog << m_streams.critical.str() << std::ends;

        m_fileLog << "[Warnings]" << std::ends;
        m_fileLog << m_streams.warning.str() << std::ends;
        m_fileLog << std::ends;

        m_fileLog << std::flush;

        m_streams.normal.clear();
        m_streams.critical.clear();
        m_streams.warning.clear();
    }

    Logger::Logger()
        : m_fileLog("Logger/Logs.txt")
    {
    }

    Logger::~Logger()
    {
        m_fileLog.close();
    }
}