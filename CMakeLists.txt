cmake_minimum_required(VERSION 3.5)
project(DeviceSupervisor)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -Wextra")

set(SOURCE_FILES main.cpp)
add_executable(DeviceSupervisor ${SOURCE_FILES})
target_link_libraries(DeviceSupervisor IoTMQTTClient Paho pthread ssl crypto PocoFoundation PocoData PocoDataSQLite PocoUtil)