/**
 * \file
 */
#ifndef DEVICE_SUPERVISOR_TASK_LOCAL_HPP
#define DEVICE_SUPERVISOR_TASK_LOCAL_HPP

#include "device_supervisor/task.hpp"

namespace device_supervisor {

class TaskLocal : public Task
{
public:
    TaskLocal(std::unique_ptr<IoT::MQTT::MQTTClient>& client, Target& target);

private:
    FactoryArguments getArguments() const override;
};

} // namespace device_supervisor

#endif // DEVICE_SUPERVISOR_TASK_LOCAL_HPP
