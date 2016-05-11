/**
 * \file
 * \brief Execute device supervisor to update data base
 */

#include <cstdlib>
#include <regex>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <queue>

#include <Poco/Logger.h>
#include <Poco/AutoPtr.h>
#include <Poco/FileChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Data/Session.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/SQLite/Connector.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Delegate.h>
#include <Poco/Task.h>
#include <Poco/TaskManager.h>

#include <IoT/MQTT/MQTTClient.h>
#include <IoT/MQTT/MQTTClientFactory.h>

#define RUN_ONLINE 0

using IoT::MQTT::MQTTClientFactory;

class SupervisorLog
{
    Poco::AutoPtr<Poco::FileChannel> fileChannel;
    Poco::AutoPtr<Poco::PatternFormatter> patternFormatter;
    Poco::AutoPtr<Poco::FormattingChannel> formattingChannel;
public:
    SupervisorLog()
    : fileChannel(new Poco::FileChannel)
    , patternFormatter(new Poco::PatternFormatter)
    {
        fileChannel->setProperty("path", "/var/tmp/device-supervisor.log");
        fileChannel->setProperty("rotation", "10 M");
        fileChannel->setProperty("archive", "timestamp");

        patternFormatter->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s: %t");
        formattingChannel.assign(new Poco::FormattingChannel(patternFormatter, fileChannel));
    }

    Poco::Logger& instance()
    {
        Poco::Logger::root().setChannel(formattingChannel);
        return Poco::Logger::get("SupervisorLog");
    }
};

/** Singleton logger */
static SupervisorLog Logger;

/**
 * \brief Treat event from server
 */
class Target
{
    /** Connect to data base */
    Poco::Data::Session sqlite_session_;
    /** Lock message treatment */
    std::mutex message_arrived_mutex_;

    using message_queue = std::queue<IoT::MQTT::MessageArrivedEvent>;
    message_queue message_queue_;
    std::atomic_bool sentinel_;
    std::unique_ptr<std::thread> message_arrived_consumer_;
    std::condition_variable message_queue_condition_;
public:

    Target()
    : sqlite_session_(Poco::Data::SessionFactory::instance().create("SQLite", "/var/www/html/db/SmartAquarium.sqlite"))
    , sentinel_ { true }
    {
        Logger.instance().information("Connect on data base");
        message_arrived_consumer_.reset(new std::thread(std::bind(&Target::ProcessMesssageQueue, this)));
    }

    ~Target()
    {
        sentinel_ = false;
        message_queue_condition_.notify_all();
        message_arrived_consumer_->join();
    }

    /**
     * \brief Consumer loop
     */
    void ProcessMesssageQueue()
    {
        while(sentinel_) {
            std::unique_lock<std::mutex> lock{message_arrived_mutex_};
            while (message_queue_.empty()) {
                message_queue_condition_.wait(lock);
                if (!sentinel_) {
                    return;
                }
            }
            Logger.instance().information("Process message queue");
            auto message_event = message_queue_.front();
            message_queue_.pop();
            lock.unlock();
            ProcessArrivedMessage(message_event);
        }
    }

    /**
     * \brief Process payload to insert on table
     * \param event Data sent by broker
     */
    void ProcessArrivedMessage(const IoT::MQTT::MessageArrivedEvent& _event)
    {
        Logger.instance().information("Process arrived message");
        const std::regex expression(R"(^smartaquarium\/((\bsensor\b)|(\bactuator\b))\/(\w+)\/level$)");
        std::smatch smatch;
        std::string topic{_event.topic};
        std::string table;
        while (std::regex_search(topic, smatch, expression)) {
            for (const auto &sm : smatch) {
                table = sm;
            }
            topic = smatch.suffix().str();
        }

        std::ostringstream oss;
        oss << "Topic: " << _event.topic << std::endl;
        oss << "Payload: " << _event.message.payload << std::endl;
        oss << "Table: " << table << std::endl;
        Logger.instance().information(oss.str());

        Poco::Data::Statement statement = (sqlite_session_ << "INSERT INTO " << table << " (value) VALUES (" << _event.message.payload << ")");
        statement.execute();
        poco_assert (statement.done());
    }

    /**
     * \brief Delegated function to treat arrived messages
     * \param _event Event released from server
     */
    void OnMessageArrived(const void*, const IoT::MQTT::MessageArrivedEvent& _event)
    {
        std::lock_guard<std::mutex> lock(message_arrived_mutex_);
        Logger.instance().information("On Message arrived");
        auto wake = message_queue_.empty();
        message_queue_.push(_event);
        if ( wake ) {
            message_queue_condition_.notify_one();
        }
    }

    /**
    * \brief Receive event from Connection lost
    * \param _sender Message provider
    * \param _event String error description
    */
    void OnConnectionLost(const void *_sender, const IoT::MQTT::ConnectionLostEvent& _event)
    {
        std::ignore = _sender;
        std::ostringstream oss;
        oss << "Connection Lost: " << _event.cause << std::endl;
        Logger.instance().error(oss.str());
    }
};

class SupervisorTask : public Poco::Task
{
    std::unique_ptr<IoT::MQTT::MQTTClient>& client_;
    Target& target_;

public:
    SupervisorTask(std::unique_ptr<IoT::MQTT::MQTTClient>& client, Target& target)
    : Task("SupervisorTask")
    , client_(client)
    , target_(target)
    {
    }

protected:
    void runTask() override
    {
        MQTTClientFactory::FactoryArguments factoryArguments;
        factoryArguments.clientId = "WebSmartAquariumDeviceSupervisor";
#if RUN_ONLINE
        factoryArguments.serverUri = "m11.cloudmqtt.com:15347";
        factoryArguments.options.username = "device_supervisor";
        factoryArguments.options.password = "devicesupervisor";
#else
        factoryArguments.serverUri = "localhost:1883";
#endif
        client_ = MQTTClientFactory::CreateMQTTClient<MQTTClientFactory::ClientType::Paho>(factoryArguments);

        client_->connectionLost += Poco::delegate(&target_, &Target::OnConnectionLost);
        client_->messageArrived += Poco::delegate(&target_, &Target::OnMessageArrived);

        client_->subscribe("smartaquarium/actuator/feeder/level", IoT::MQTT::QoS::AT_LEAST_ONCE);
        client_->subscribe("smartaquarium/sensor/ph/level", IoT::MQTT::QoS::AT_LEAST_ONCE);
        client_->subscribe("smartaquarium/sensor/temperature/level", IoT::MQTT::QoS::AT_LEAST_ONCE);
        client_->subscribe("smartaquarium/sensor/light/level", IoT::MQTT::QoS::AT_LEAST_ONCE);
    }
};

/**
 * \brief Application
 */
class DeviceSupervisorApp : public Poco::Util::ServerApplication
{
protected:

    int main(const ArgVec&) override
    {
        Poco::Data::SQLite::Connector::registerConnector();

        Poco::TaskManager taskManager;
        std::unique_ptr<IoT::MQTT::MQTTClient> client;
        Target taget;

        taskManager.start(new SupervisorTask(client, taget));

        waitForTerminationRequest();

        taskManager.cancelAll();
        taskManager.joinAll();

        return Poco::Util::Application::EXIT_OK;
    }

};

POCO_SERVER_MAIN(DeviceSupervisorApp);
