cmake_minimum_required(VERSION 3.9)
project(BeitaGo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_AUTOMOC ON)

set(GOOGLE_TEST_DIR lib/googletest)
add_subdirectory(${GOOGLE_TEST_DIR})

find_package(Qt5Core)
find_package(Qt5Widgets)
set(QT5_LIBRARIES Qt5::Core Qt5::Widgets)

include_directories(./include)

add_library(BeitaGoEngine src/Engine/Engine.cpp include/Engine.h)

add_executable(BeitaGo src/View/Main.cpp)
target_link_libraries(BeitaGo BeitaGoEngine ${QT5_LIBRARIES})

add_executable(BeitaGoTest test/TestEngine.cpp)
target_include_directories(BeitaGoTest PRIVATE ${GOOGLE_TEST_DIR}/googletest/include)
target_link_libraries(BeitaGoTest BeitaGoEngine gtest)