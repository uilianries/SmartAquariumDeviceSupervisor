/**
 * \file
 * \brief Load Configuration from XML file
 */
#include "device_supervisor/task_configuration.hpp"

#include <Poco/AutoPtr.h>
#include <Poco/Util/XMLConfiguration.h>

namespace device_supervisor {

TaskConfiguration::TaskConfiguration(std::unique_ptr<IoT::MQTT::MQTTClient>& client, Target& target)
    : Task(client, target)
{
}

FactoryArguments TaskConfiguration::getArguments() const
{
    Poco::AutoPtr<Poco::Util::XMLConfiguration> file(new Poco::Util::XMLConfiguration("/etc/smartaquarium/device-supervisor.xml"));

    static FactoryArguments arguments;

    arguments.clientId = file->getString("clientid");
    arguments.serverUri = file->getString("server") + ":" + file->getString("port");
    arguments.options.username = file->getString("username");
    arguments.options.password = file->getString("password");

    return arguments;
}

} // namespace device_supervisor
