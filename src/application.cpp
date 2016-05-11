/**
 * \brief
 */
#include "device_supervisor/application.hpp"

#include <memory>

#include <Poco/Data/SQLite/Connector.h>
#include <Poco/TaskManager.h>

#include <IoT/MQTT/MQTTClient.h>

#include "device_supervisor/target.hpp"
#include "device_supervisor/task_remote.hpp"
#include "device_supervisor/logger.hpp"

namespace device_supervisor {

int Application::main(const ArgVec&)
{
    Poco::Data::SQLite::Connector::registerConnector();

    Poco::TaskManager taskManager;
    std::unique_ptr<IoT::MQTT::MQTTClient> client;
    device_supervisor::Target target;

    Logger::instance().information("Start Task Manager");
    taskManager.start(new device_supervisor::TaskRemote(client, target));

    Logger::instance().information("Wait For Termination");
    waitForTerminationRequest();

    Logger::instance().information("Termination has been requested");

    taskManager.cancelAll();
    taskManager.joinAll();

    return Poco::Util::Application::EXIT_OK;
}

} // namespace device_supervisor
