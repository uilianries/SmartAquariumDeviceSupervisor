/**
 * \file
 */
#include "device_supervisor/task_remote.hpp"

namespace device_supervisor {

TaskRemote::TaskRemote(std::unique_ptr<IoT::MQTT::MQTTClient>& client, Target& target)
    : Task(client, target)
{
}

FactoryArguments TaskRemote::getArguments() const
{
    static FactoryArguments arguments;

    arguments.clientId = "WebSmartAquariumDeviceSupervisor";
    arguments.serverUri = "m11.cloudmqtt.com:15347";
    arguments.options.username = "device_supervisor";
    arguments.options.password = "devicesupervisor";

    return arguments;
}

} // namespace device_supervisor
