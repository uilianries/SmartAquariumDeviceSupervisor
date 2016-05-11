/**
 * \file
 */
#include "device_supervisor/task.hpp"
#include "device_supervisor/logger.hpp"

#include <Poco/Delegate.h>

namespace device_supervisor {

Task::Task(std::unique_ptr<IoT::MQTT::MQTTClient>& client, Target& target)
: Poco::Task("SupervisorTask")
, client_(client)
, target_(target)
{
    Logger::instance().information("Create Task");
}

void Task::runTask()
{
    using IoT::MQTT::MQTTClientFactory;
    using ClientType = IoT::MQTT::MQTTClientFactory::ClientType;

    auto factoryArguments = getArguments();
    Logger::instance().information("Create MQTT Client");
    client_ = MQTTClientFactory::CreateMQTTClient<ClientType::Paho>(factoryArguments);

    Logger::instance().information("Delegate callback function");
    client_->connectionLost += Poco::delegate(&target_, &Target::OnConnectionLost);
    client_->messageArrived += Poco::delegate(&target_, &Target::OnMessageArrived);

    Logger::instance().information("Subscribe topics");
    client_->subscribeMany({{"smartaquarium/actuator/feeder/level", IoT::MQTT::QoS::AT_LEAST_ONCE},
                            {"smartaquarium/sensor/ph/level", IoT::MQTT::QoS::AT_LEAST_ONCE},
                            {"smartaquarium/sensor/temperature/level", IoT::MQTT::QoS::AT_LEAST_ONCE},
                            {"smartaquarium/sensor/light/level", IoT::MQTT::QoS::AT_LEAST_ONCE}});
}

} // namespace device_supervisor

