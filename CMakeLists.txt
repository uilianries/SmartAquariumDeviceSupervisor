cmake_minimum_required(VERSION 2.8)
project(DeviceSupervisor CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -Wextra -Wshadow")
set(SERVICE_SCRIPT smartaquarium-device-supervisor)

include(build/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(include)

file(GLOB SOURCE_FILES
        src/*.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${CMAKE_PROJECT_NAME} IoTMQTTClient Paho ${CONAN_LIBS})

install(TARGETS ${CMAKE_PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(FILES ${SERVICE_SCRIPT}
  DESTINATION /etc/init.d
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(FILES device-supervisor.xml
  DESTINATION /etc/smartaquarium
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install(CODE "execute_process(COMMAND /usr/sbin/update-rc.d ${SERVICE_SCRIPT} defaults)")
