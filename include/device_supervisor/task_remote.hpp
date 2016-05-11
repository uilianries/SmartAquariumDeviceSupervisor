/**
 * \file
 */
#ifndef DEVICE_SUPERVISOR_TASK_REMOTE_HPP
#define DEVICE_SUPERVISOR_TASK_REMOTE_HPP

#include "device_supervisor/task.hpp"

namespace device_supervisor {

class TaskRemote : public Task
{
public:
    TaskRemote(std::unique_ptr<IoT::MQTT::MQTTClient>& client, Target& target);

private:
    FactoryArguments getArguments() const override;
};

} // namespace device_supervisor

#endif // DEVICE_SUPERVISOR_TASK_REMOTE_HPP
