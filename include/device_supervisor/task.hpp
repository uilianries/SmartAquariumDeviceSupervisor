/** 
 * \file
 */
#ifndef DEVICE_SUPERVISOR_TASK_HPP
#define DEVICE_SUPERVISOR_TASK_HPP

#include <memory>

#include <Poco/Task.h>
#include <IoT/MQTT/MQTTClient.h>
#include <IoT/MQTT/MQTTClientFactory.h>

#include "device_supervisor/target.hpp"

using FactoryArguments = IoT::MQTT::MQTTClientFactory::FactoryArguments;

namespace device_supervisor {

class Task : public Poco::Task {
public:
    Task(std::unique_ptr<IoT::MQTT::MQTTClient>& client, Target& target);

protected:
    virtual FactoryArguments getArguments() const = 0;

    void runTask() override;

private:
    std::unique_ptr<IoT::MQTT::MQTTClient>& client_;
    Target& target_;
};

} // namespace device_supervisor

#endif // DEVICE_SUPERVISOR_TASK_HPP
