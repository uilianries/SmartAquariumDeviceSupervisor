/**
 * \file
 */
#ifndef DEVICE_SUPERVISOR_APP_HPP
#define DEVICE_SUPERVISOR_APP_HPP

#include <Poco/Util/ServerApplication.h>

using ArgVec = Poco::Util::Application::ArgVec;

namespace device_supervisor {

/**
 * \brief Application
 */
class Application : public Poco::Util::ServerApplication
{
protected:
    /**
     * \brief Execute main application
     */
    int main(const ArgVec&) override;
};

} // namespace device_supervisor


#endif // DEVICE_SUPERVISOR_APP_HPP
