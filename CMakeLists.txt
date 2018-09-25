cmake_minimum_required(VERSION 3.0)
project(BeitaGo)

set(CMAKE_CXX_STANDARD 11)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-std=c++11")
elseif(MSVC)
    set(gtest_force_shared_crt ON)
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# This is some magic for Qt.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

message("BeitaGo: Adding googletest")
set(GOOGLE_TEST_DIR lib/googletest)
add_subdirectory(${GOOGLE_TEST_DIR})

if(BEITAGO_QT)
    message("BeitaGo: Adding Qt")
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
    set(QT5_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets)
endif()

message("BeitaGo: Adding dlib")
set(DLIB_DIR lib/dlib)
add_subdirectory(${DLIB_DIR})

message("BeitaGo: Making projects")
# All projects will use this include directory. It should hold files made public by the engine.
include_directories(./include)

add_library(BeitaGoEngine SHARED src/Engine/Engine.cpp include/Engine.h src/Engine/Player.cpp include/Player.h include/Constants.h src/Engine/HumanPlayer.cpp include/HumanPlayer.h include/Grid2.h src/Engine/Grid2.cpp src/Engine/Board.cpp include/Board.h src/Engine/AIPlayer.cpp include/AIPlayer.h src/Engine/DumbAIPlayer.cpp include/DumbAIPlayer.h src/Engine/MoveHistoryEntry.cpp include/MoveHistoryEntry.h src/Engine/MonteCarloAIPlayer.cpp include/MonteCarloAIPlayer.h src/Engine/DeepLearning/MonteCarloTree.cpp src/Engine/DeepLearning/MonteCarloTree.h src/Engine/DeepLearning/MonteCarloNode.cpp src/Engine/DeepLearning/MonteCarloNode.h)

if(BEITAGO__QT)
    add_executable(BeitaGoQt src/View/Main.cpp ui/MainWindow.cpp ui/MainWindow.h ui/MainWindow.ui ui/BoardView.cpp ui/BoardView.h)
    # All QObjects sit in the UI folder. While you technically can include .cpp files from here, you
    # won't get far with that.
    target_include_directories(BeitaGoQt PRIVATE ui)
    target_link_libraries(BeitaGoQt BeitaGoEngine ${QT5_LIBRARIES})
endif()

add_executable(BeitaGoASCII src/Ascii/Main.cpp)
target_link_libraries(BeitaGoASCII BeitaGoEngine)

add_executable(BeitaGoTest test/TestMain.cpp test/TestEngine.h test/TestGrid2.h)
target_include_directories(BeitaGoTest PRIVATE ${GOOGLE_TEST_DIR}/googletest/include)
target_link_libraries(BeitaGoTest BeitaGoEngine gtest)

add_test(NAME BeitaGoTestingStuff COMMAND BeitaGoTest)

add_executable(BeitaGoGTP src/GTP/Main.cpp src/GTP/GTPEngine.h src/GTP/GTPEngine.cpp)
target_link_libraries(BeitaGoGTP BeitaGoEngine)

add_executable(TestDLIB test/TestDLIB.cpp)
target_include_directories(TestDLIB PRIVATE ${DLIB_DIR})
target_link_libraries(TestDLIB dlib)

# This is just for testing variables. It clogs up the output though, so it's commented.
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()


# On Windows, Qt needs this deployment options to add the right DLLs next to it. I dunno about
# other platforms though.
# Also, on Visual Studio, it uses an extra suffix here, so add that.
IF(WIN32 AND BEITAGO_QT)
    message("BeitaGo: Making BeitaGoQt post-build deployment")
    IF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(BEITA_GO_DEBUG_STATUS debug)
    ELSE()
        set(BEITA_GO_DEBUG_STATUS release)
    ENDIF()
    if(MSVC)
        #add_custom_command(TARGET BeitaGo POST_BUILD COMMAND ${CMAKE_PREFIX_PATH}/bin/windeployqt.exe ARGS $(OutDir)BeitaGoQt.exe COMMENT "Deploying Qt Windows ${BEITA_GO_DEBUG_STATUS} libraries...")
        set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)
    #ELSE()
        #add_custom_command(TARGET BeitaGo POST_BUILD COMMAND ${CMAKE_PREFIX_PATH}/bin/windeployqt.exe ARGS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${MSVC_EXTRA_OUTPUT_DIR}/BeitaGoQt.exe --${BEITA_GO_DEBUG_STATUS} --no-system-d3d-compiler COMMENT "Deploying Qt Windows ${BEITA_GO_DEBUG_STATUS} libraries...")
    ENDIF()
    add_custom_command(TARGET BeitaGoQt POST_BUILD COMMAND ${CMAKE_PREFIX_PATH}/bin/windeployqt.exe ARGS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${MSVC_EXTRA_OUTPUT_DIR}/BeitaGoQt.exe --${BEITA_GO_DEBUG_STATUS} --no-system-d3d-compiler COMMENT "Deploying Qt Windows ${BEITA_GO_DEBUG_STATUS} libraries...")
ENDIF()