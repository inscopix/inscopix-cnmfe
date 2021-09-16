#include "isxLogger.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

namespace isx {

    std::unique_ptr<Logger> Logger::s_instance;

    class Logger::Impl : public std::enable_shared_from_this<Logger::Impl>
    {
        public:
            Impl(const std::string & inLogFileName, const std::string & inAppName, const std::string & inAppVersion)
            {
                m_filename = inLogFileName;
                m_appName = inAppName;
                m_appVersion = inAppVersion;

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
                return m_appName;
            }

            const std::string &
            getAppVersion() const
            {
                return m_appVersion;
            }

        private:
            std::string m_filename;
            std::string m_appName;
            std::string m_appVersion;
    };

    // get current date & time, format is YYYY-MM-DD HH:mm:ss.xxx (xxx represents milliseconds)
    std::string currentDateTime() {
        using namespace std::chrono;

        system_clock::time_point now = system_clock::now();
        milliseconds ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

        time_t timer = system_clock::to_time_t(now);
        std::ostringstream oss;
        char timeStr[20];
        if (std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&timer))) {
            oss << timeStr; // HH:MM:SS
        }
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    // returns the name of the operating system
    std::string getOsName()
    {
        #ifdef _WIN32
        return "Windows 32-bit";
        #elif _WIN64
        return "Windows 64-bit";
        #elif __APPLE__ || __MACH__
        return "Mac OSX";
        #elif __linux__
        return "Linux";
        #elif __FreeBSD__
        return "FreeBSD";
        #elif __unix || __unix__
        return "Unix";
        #else
        return "Other";
        #endif
    }

    Logger::Logger(const std::string & inLogFileName, const std::string & inAppName, const std::string & inAppVersion)
    {
        m_pImpl.reset(new Impl(inLogFileName, inAppName, inAppVersion));
    }

    Logger::~Logger()
    {
    }

    void
    Logger::initialize(const std::string & inLogFileName, const std::string & inAppName, const std::string & inAppVersion)
    {
        if(inLogFileName.empty())
        {
            return;
        }

        if (!isInitialized())
        {
            s_instance.reset(new Logger(inLogFileName, inAppName, inAppVersion));
        }

        logSystemInfo();
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
            const std::string message = "[" + currentDateTime() + "]"
                                      + "[" + instance()->m_pImpl->getAppName() + "]"
                                      + "[" + isx::logTypeNameMap.at(logType) + "]"
                                      + " " +  text;
            std::cout << message;
            std::cout << std::flush;
            instance()->m_pImpl->log(message);
        }
    }

    void
    Logger::logSystemInfo()
    {
        if (isInitialized())
        {
            try {
                log(getAppName() + " version: " + getAppVersion() + '\n', LogType_t::SYSTEM);
                log("Operating system: " + getOsName() + '\n', LogType_t::SYSTEM);
                log("Number of cores: " + std::to_string(std::thread::hardware_concurrency())  + '\n', LogType_t::SYSTEM);
            }
            catch(...)
            {
                const std::string message = "System information could not be retrieved";
                log(message + "\n", LogType_t::WARNING);
            }
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

    const std::string &
    Logger::getAppVersion()
    {
        if (isInitialized())
        {
            return instance()->m_pImpl->getAppVersion();
        }

        static std::string emptyString;
        return emptyString;
    }
}
