/**
 * \file
 */
#include "device_supervisor/target.hpp"

#include <sstream>
#include <regex>

#include <Poco/Data/SessionFactory.h>

#include "device_supervisor/logger.hpp"

namespace device_supervisor {

Target::Target()
    : sqlite_session_(Poco::Data::SessionFactory::instance().create("SQLite", "/var/www/html/db/SmartAquarium.sqlite"))
    , sentinel_{ true }
{
    Logger::instance().information("Connect on data base");
    message_arrived_consumer_.reset(new std::thread(std::bind(&Target::ProcessMesssageQueue, this)));
}

Target::~Target()
{
    sentinel_ = false;
    message_queue_condition_.notify_all();
    message_arrived_consumer_->join();
}

void Target::ProcessMesssageQueue()
{
    while (sentinel_) {
        std::unique_lock<std::mutex> lock{ message_arrived_mutex_ };
        while (message_queue_.empty()) {
            message_queue_condition_.wait(lock);
            if (!sentinel_) {
                return;
            }
        }
        auto message_event = message_queue_.front();
        message_queue_.pop();
        lock.unlock();
        ProcessArrivedMessage(message_event);
    }
}

void Target::ProcessArrivedMessage(const IoT::MQTT::MessageArrivedEvent& _event)
{
    const std::regex expression(R"(^smartaquarium\/((\bsensor\b)|(\bactuator\b))\/(\w+)\/level$)");
    std::smatch smatch;
    std::string topic{ _event.topic };
    std::string table;
    while (std::regex_search(topic, smatch, expression)) {
        for (const auto& sm : smatch) {
            table = sm;
        }
        topic = smatch.suffix().str();
    }

    std::ostringstream oss;
    oss << "Topic: " << _event.topic << std::endl;
    oss << "Payload: " << _event.message.payload << std::endl;
    oss << "Table: " << table << std::endl;
    Logger::instance().information(oss.str());

    Poco::Data::Statement statement = (sqlite_session_ << "INSERT INTO " << table << " (value) VALUES (" << _event.message.payload << ")");
    statement.execute();
    poco_assert(statement.done());
}

void Target::OnMessageArrived(const void*, const IoT::MQTT::MessageArrivedEvent& _event)
{
    std::lock_guard<std::mutex> lock(message_arrived_mutex_);
    auto wake = message_queue_.empty();
    message_queue_.push(_event);
    if (wake) {
        message_queue_condition_.notify_one();
    }
}

void Target::OnConnectionLost(const void* _sender, const IoT::MQTT::ConnectionLostEvent& _event)
{
    std::ignore = _sender;
    std::ostringstream oss;
    oss << "Connection Lost: " << _event.cause << std::endl;
    Logger::instance().error(oss.str());
}

} // namespace device_supervisor
