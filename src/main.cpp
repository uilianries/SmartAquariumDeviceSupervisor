/**
 * \file
 * \brief Execute device supervisor to update data base
 */
#include <iostream>
#include <Poco/Util/ServerApplication.h>
#include "device_supervisor/application.hpp"

POCO_SERVER_MAIN(device_supervisor::Application);