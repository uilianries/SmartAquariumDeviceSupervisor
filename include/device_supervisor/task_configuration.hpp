/**
 * \file
 * \brief Load configuration from XML file
 */
#ifndef DEVICE_SUPERVISOR_CONFIG_H_
#define DEVICE_SUPERVISOR_CONFIG_H_

#include "device_supervisor/task.hpp"

namespace device_supervisor {

class TaskConfiguration : public Task
{
    public:
    TaskConfiguration(std::unique_ptr<IoT::MQTT::MQTTClient>& client, Target& target);

    FactoryArguments getArguments() const override;
};

} // namespace device_supervisor

#endif // DEVICE_SUPERVISOR_CONFIG_H_
