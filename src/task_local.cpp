/**
 * \file
 */
#include "device_supervisor/task_local.hpp"

namespace device_supervisor {

TaskLocal::TaskLocal(std::unique_ptr<IoT::MQTT::MQTTClient>& client, Target& target)
: Task(client, target)
{
}

FactoryArguments TaskLocal::getArguments() const
{
    static FactoryArguments arguments;

    arguments.clientId = "LocalSmartAquariumDeviceSupervisor";
    arguments.serverUri = "localhost:1883";

    return arguments;
}

} // namespace device_supervisor
