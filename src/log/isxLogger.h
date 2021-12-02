#ifndef ISX_LOGGER_H
#define ISX_LOGGER_H

#include <memory>
#include <string>
#include <map>

namespace isx {

    /// Types of logging messages
    enum class LogType_t
    {
        INFO = 0,
        WARNING,
        ERROR,
        DEBUG,
        SYSTEM
    };

    /// Mapping from log types to log type names
    const static std::map<LogType_t, std::string> logTypeNameMap =
    {
        {LogType_t::INFO, "INFO"},
        {LogType_t::WARNING, "WARN"},
        {LogType_t::ERROR, "ERROR"},
        {LogType_t::DEBUG, "DEBUG"},
        {LogType_t::SYSTEM, "SYSTEM"}
    };

    /// A class implementing a singleton Logger for application-wide logging
    ///
    class Logger
    {
        public:
            /// destructor
            ///
            ~Logger();

            /// Singleton initializer
            /// \param inLogFileName if empty, nothing will be logged
            /// \param inAppName string representing the name of the app (will appear on every log message)
            /// \param inAppVersion string representing the version of the app (will only appear when logging system info)
            /// \param inVerbose bool indicating whether to display progress in the console
            static
            void
            initialize(
                const std::string & inLogFileName,
                const std::string & inAppName,
                const std::string & inAppVersion,
                const bool inVerbose);

            /// Check if singleton has been initialized
            /// \return bool indicating the above
            ///
            static
            bool
            isInitialized();

            /// \return pointer to the Logger singleton instance
            ///
            static
            Logger *
            instance();

            /// Add a line to the log
            /// \param text text to be logged
            /// \param logType type of log message
            static
            void
            log(const std::string & text, const LogType_t & logType);

            /// Add system information to the log
            static
            void
            logSystemInfo();

            /// \return log filename
            ///
            static
            const std::string &
            getLogFileName();

            /// \return app name
            ///
            static
            const std::string &
            getAppName();

            /// \return app version
            ///
            static
            const std::string &
            getAppVersion();

            /// \return whether logger is verbose
            ///
            static
            const bool
            isVerbose();

        private:
            Logger(
                const std::string & inLogFileName,
                const std::string & inAppName,
                const std::string & inAppVersion,
                const bool inVerbose);
            Logger(const Logger & other) = delete;
            const Logger & operator=(const Logger & other) = delete;

            class Impl;
            std::shared_ptr<Impl> m_pImpl;

            static std::unique_ptr<Logger> s_instance;
    }; // logger class
} // namespace isx

#endif // def ISX_LOGGER_H
