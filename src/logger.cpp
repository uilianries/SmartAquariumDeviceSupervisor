/**
 * \file
 */
#include "device_supervisor/logger.hpp"

namespace device_supervisor {

Logger::Logger()
 : fileChannel(new Poco::FileChannel)
 , patternFormatter(new Poco::PatternFormatter)
{
    fileChannel->setProperty("path", "/var/tmp/device-supervisor.log");
    fileChannel->setProperty("rotation", "10 M");
    fileChannel->setProperty("archive", "timestamp");

    patternFormatter->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s: %t");
    formattingChannel.assign(new Poco::FormattingChannel(patternFormatter, fileChannel));
    Poco::Logger::root().setChannel(formattingChannel);
}

Poco::Logger& Logger::instance()
{
    static device_supervisor::Logger supervisor_logger;
    return Poco::Logger::get("SupervisorLogger");
}

} // namespace device_supervisor
