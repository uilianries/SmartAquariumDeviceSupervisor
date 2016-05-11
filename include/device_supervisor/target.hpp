//
// Created by uilian on 5/11/16.
//

#ifndef DEVICE_SUPERVISOR_TARGET_HPP
#define DEVICE_SUPERVISOR_TARGET_HPP

#include <mutex>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <thread>

#include <Poco/Data/Session.h>
#include <IoT/MQTT/MQTTClient.h>

namespace device_supervisor {

class Target
{
public:
    Target();

    ~Target();

    /**
     * \brief Consumer loop
     */
    void ProcessMesssageQueue();

    /**
     * \brief Process payload to insert on table
     * \param event Data sent by broker
     */
    void ProcessArrivedMessage(const IoT::MQTT::MessageArrivedEvent& _event);

    /**
     * \brief Delegated function to treat arrived messages
     * \param _event Event released from server
     */
    void OnMessageArrived(const void*, const IoT::MQTT::MessageArrivedEvent& _event);

    /**
    * \brief Receive event from Connection lost
    * \param _sender Message provider
    * \param _event String error description
    */
    void OnConnectionLost(const void *_sender, const IoT::MQTT::ConnectionLostEvent& _event);

private:
    /** Connect to data base */
    Poco::Data::Session sqlite_session_;
    /** Lock message treatment */
    std::mutex message_arrived_mutex_;

    using message_queue = std::queue<IoT::MQTT::MessageArrivedEvent>;
    message_queue message_queue_;
    std::atomic_bool sentinel_;
    std::unique_ptr<std::thread> message_arrived_consumer_;
    std::condition_variable message_queue_condition_;
};

} // namespace device_supervisor

#endif // DEVICE_SUPERVISOR_TARGET_HPP
