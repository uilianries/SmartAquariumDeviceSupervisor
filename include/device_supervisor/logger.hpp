/**
 * \file
 */
#ifndef SUPERVISOR_LOGGER_HPP
#define SUPERVISOR_LOGGER_HPP

#include <Poco/Logger.h>
#include <Poco/AutoPtr.h>
#include <Poco/FileChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>

namespace device_supervisor {

/**
 * \brief Create Application Logger
 */
class Logger {
public:
    /**
    * \brief Copy constructor not allowed
    */
    Logger(const Logger&) = delete;

    /**
     * \brief Assign operator not allowed
     */
    Logger& operator=(const Logger&) = delete;

    /**
     * \brief Get singleton instance
     */
    static Poco::Logger& instance();

private:
    /**
     * \brief Logger instance
     */
    Logger();

    Poco::AutoPtr<Poco::FileChannel> fileChannel;
    Poco::AutoPtr<Poco::PatternFormatter> patternFormatter;
    Poco::AutoPtr<Poco::FormattingChannel> formattingChannel;
};

} // namespace device_supervisor

#endif // SUPERVISOR_LOGGER_HPP
