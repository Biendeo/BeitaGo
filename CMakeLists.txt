cmake_minimum_required(VERSION 3.9)
project(BeitaGo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# This is some magic for Qt.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(GOOGLE_TEST_DIR lib/googletest)
add_subdirectory(${GOOGLE_TEST_DIR})

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
set(QT5_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets)

# All projects will use this include directory. It should hold files made public by the engine.
include_directories(./include)

add_library(BeitaGoEngine SHARED src/Engine/Engine.cpp include/Engine.h)

add_executable(BeitaGo src/View/Main.cpp ui/MainWindow.cpp ui/MainWindow.h ui/MainWindow.ui)
# All QObjects sit in the UI folder. While you technically can include .cpp files from here, you
# won't get far with that.
target_include_directories(BeitaGo PRIVATE ui)
target_link_libraries(BeitaGo BeitaGoEngine ${QT5_LIBRARIES})

add_executable(BeitaGoTest test/TestEngine.cpp)
target_include_directories(BeitaGoTest PRIVATE ${GOOGLE_TEST_DIR}/googletest/include)
target_link_libraries(BeitaGoTest BeitaGoEngine gtest)

# This is just for testing variables. It clogs up the output though, so it's commented.

get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()


# On Windows, Qt needs this deployment options to add the right DLLs next to it. I dunno about
# other platforms though.
IF(WIN32)
    IF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(BEITA_GO_DEBUG_STATUS debug)
    ELSE()
        set(BEITA_GO_DEBUG_STATUS release)
    ENDIF()
    add_custom_command(TARGET BeitaGo POST_BUILD COMMAND ${CMAKE_PREFIX_PATH}/bin/windeployqt.exe ARGS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BeitaGo.exe --${BEITA_GO_DEBUG_STATUS} --no-system-d3d-compiler COMMENT "Deploying Qt Windows ${BEITA_GO_DEBUG_STATUS} libraries...")
ENDIF()