#include "isxLogger.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace isx {

    std::unique_ptr<Logger> Logger::s_instance;

    class Logger::Impl : public std::enable_shared_from_this<Logger::Impl>
    {
        public:
            Impl(const std::string & inLogFileName, const std::string & inAppName)
            {
                m_filename = inLogFileName;
                m_appname = inAppName;

                // remove previous log file if existent
                std::remove(m_filename.c_str());
            }

            void log(const std::string & inText)
            {
                // Note: there are no checks here (i.e. good()) on purpose.
                // Even if we get a bad stream, we can't "log" it.
                // By default, streams don't throw exceptions so in the worst case-scenario it will fail silently.
                std::ofstream file(m_filename, std::ios::out | std::ios::app);
                file << inText;
                file.flush();
                file.close();
            }

            const std::string &
            getLogFileName() const
            {
                return m_filename;
            }

            const std::string &
            getAppName() const
            {
                return m_appname;
            }

        private:
            std::string m_filename;
            std::string m_appname;
    };

    // get current date & time, format is YYYY-MM-DD HH:mm:ss.xxx (xxx represents milliseconds)
    std::string currentDateTime() {
        using namespace std::chrono;

        system_clock::time_point now = system_clock::now();
        milliseconds ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

        time_t timer = system_clock::to_time_t(now);
        std::tm bt = *std::localtime(&timer);

        std::ostringstream oss;
        oss << std::put_time(&bt, "%Y-%m-%d %H:%M:%S"); // HH:MM:SS
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    // pad string with spaces at the end
    void padTo(std::string &str, const size_t num, const char paddingChar = ' ')
    {
        if(num > str.size())
        {
            str.insert(str.size(), num - str.size(), paddingChar);
        }
    }

    Logger::Logger(const std::string & inLogFileName, const std::string & inAppName)
    {
        m_pImpl.reset(new Impl(inLogFileName, inAppName));
    }

    Logger::~Logger()
    {
    }

    void
    Logger::initialize(const std::string & inLogFileName, const std::string & inAppName)
    {
        if(inLogFileName.empty())
        {
            return;
        }

        if (!isInitialized())
        {
            s_instance.reset(new Logger(inLogFileName, inAppName));
        }
    }

    bool
    Logger::isInitialized()
    {
        return (s_instance != nullptr);
    }

    Logger *
    Logger::instance()
    {
        if (isInitialized())
        {
            return s_instance.get();
        }
        return nullptr;
    }

    void
    Logger::log(const std::string & text, const LogType_t & logType)
    {
        if (isInitialized())
        {
            std::string logTypeStr = "[" + isx::logTypeNameMap.at(logType) + "]";
            padTo(logTypeStr, 7);

            const std::string message = "[" + currentDateTime() + "]"
                                      + "[" + instance()->m_pImpl->getAppName() + "]"
                                      + logTypeStr
                                      + " " +  text;
            std::cout << message;
            std::cout << std::flush;
            instance()->m_pImpl->log(message);
        }
    }

    const std::string &
    Logger::getLogFileName()
    {
        if (isInitialized())
        {
            return instance()->m_pImpl->getLogFileName();
        }

        static std::string emptyString;
        return emptyString;
    }

    const std::string &
    Logger::getAppName()
    {
        if (isInitialized())
        {
            return instance()->m_pImpl->getAppName();
        }

        static std::string emptyString;
        return emptyString;
    }
}
