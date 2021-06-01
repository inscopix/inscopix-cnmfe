#ifndef ISX_LOG_H
#define ISX_LOG_H

#include "isxLogger.h"
#include <iostream>
#include <sstream>
#include <utility>

/// \def ISX_LOG_INFO(...)
///
/// Logs the arguments as strings in an info message.
#define ISX_LOG_INFO(...) isx::internal::log_(isx::LogType_t::INFO, __VA_ARGS__)

/// \def ISX_LOG_WARNING(...)
///
/// Logs the arguments as strings in a warning message.
#define ISX_LOG_WARNING(...) isx::internal::log_(isx::LogType_t::WARNING, __VA_ARGS__)

/// \def ISX_LOG_ERROR(...)
///
/// Logs the arguments as strings in an error message.
#define ISX_LOG_ERROR(...) isx::internal::log_(isx::LogType_t::ERROR, __VA_ARGS__)

/// \def ISX_LOG_DEBUG(...)
///
/// Logs the arguments as strings in a debug message.
#define ISX_LOG_DEBUG(...) isx::internal::log_(isx::LogType_t::DEBUG, __VA_ARGS__)

/// \def ISX_LOG_SYSTEM(...)
///
/// Logs the arguments as strings in a system message.
#define ISX_LOG_SYSTEM(...) isx::internal::log_(isx::LogType_t::SYSTEM, __VA_ARGS__)

// DO NOT USE THESE FUNCTIONS DIRECTLY, USE MACROS ABOVE INSTEAD
namespace isx
{
    namespace internal
    {
        /// Stopping condition for recursive streamVarArgs.
        ///
        /// \param  strm        The stream to which to append.
        inline void streamVarArgs(std::ostringstream& strm)
        {
        }

        /// Appends variadic arguments to an output string stream.
        ///
        /// \param  strm        The stream to which to append.
        /// \param  first       The next argument to append.
        /// \param  rest        The rest of the arguments to append.
        template<typename First, typename ...Rest>
        void streamVarArgs(std::ostringstream& strm, First && first, Rest && ...rest)
        {
            strm << std::forward<First>(first);
            streamVarArgs(strm, std::forward<Rest>(rest)...);
        }

        /// Converts variadic arguments to a string using an output string stream.
        ///
        /// \param  rest        The arguments to convert to a string.
        /// \return             The converted string.
        template<typename ...Rest>
        std::string varArgsToString(Rest && ...rest)
        {
            std::ostringstream strm;
            streamVarArgs(strm, std::forward<Rest>(rest)...);
            return strm.str();
        }

        /// Appends variadic arguments to a platform specific output buffer.
        ///
        /// \param  rest    The arguments to append to the output buffer.
        template<typename ...Rest>
        void log_(LogType_t logType, Rest && ...rest)
        {
            std::string str = isx::internal::varArgsToString(std::forward<Rest>(rest)..., "\n");
            Logger::log(str, logType);
        }
    } // namespace internal
} // namespace isx

#endif  // def ISX_LOG_H
