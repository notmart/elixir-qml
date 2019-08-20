cmake_minimum_required(VERSION 3.5)
project(qml)

SET(CMAKE_CXX_STANDARD 11)


find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE COMPONENTS Core Qml Quick Gui Widgets)
set(CMAKE_AUTOMOC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/priv)
file(GLOB QML_SRC src/*.cpp)

#set(CMAKE_C_FLAGS "-g -O3 -pedantic -Wall -Wextra -Wno-unused-parameter -std=c99 -undefined -dynamic_lookup -dynamiclib")
set(CMAKE_CXX_FLAGS "-g -O3 -fPIC -shared -std=c++11  -fpermissive")
set(CMAKE_LD_FLAGS "-shared")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(${CMAKE_SOURCE_DIR}/cmake/FindErlang.cmake)

find_package(Erlang REQUIRED)

include_directories(
  SYSTEM
  ${ERLANG_EI_INCLUDE_DIR}
  ${ERLANG_EI_DIR}/src
)

link_directories(
  ${ERLANG_EI_LIBRARY_DIR}
)

add_executable(qml.so ${QML_SRC})

target_link_libraries(qml.so
                      ${ERLANG_EI_LIBRARIES}
                      Qt5::Core
                      Qt5::Qml
                      Qt5::Quick
                      Qt5::Gui
                      Qt5::Widgets)
