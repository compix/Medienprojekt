cmake_minimum_required (VERSION 2.6)

project(Game)

if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug")
endif()

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${Game_SOURCE_DIR}/Game)
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Game)
#set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wextra -Wall -pedantic -Wno-unused-parameter -Wno-shadow")

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Box2D/Box2D)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/cppformat)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/enet/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/entityx)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/jsoncpp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SFML)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/TGUI/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
endif()

file(GLOB_RECURSE BOMBAMAN_SOURCE
         RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
                 Source/*.cpp
                 Source/*.h*)

set(EXECUTABLE_NAME "Game")
add_executable(${EXECUTABLE_NAME} Source/Main.cpp ${BOMBAMAN_SOURCE})

add_subdirectory(ThirdParty/Box2D/Box2D)
add_subdirectory(ThirdParty/cppformat)
add_subdirectory(ThirdParty/enet)
add_subdirectory(ThirdParty/entityx)
add_subdirectory(ThirdParty/jsoncpp)
add_subdirectory(ThirdParty/SFML)
add_subdirectory(ThirdParty/TGUI)

add_definitions( -DSFML_STATIC )
add_definitions( -DHAS_STD_MAKE_UNIQUE )
#set(SFML_STATIC_LIBRARIES TRUE)

find_package(SFML COMPONENTS graphics window system audio)

include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} Box2D)
target_link_libraries(${EXECUTABLE_NAME} format)
target_link_libraries(${EXECUTABLE_NAME} enet)
target_link_libraries(${EXECUTABLE_NAME} entityx)
target_link_libraries(${EXECUTABLE_NAME} jsoncpp_lib_static)
target_link_libraries(${EXECUTABLE_NAME} tgui)
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
target_link_libraries(${EXECUTABLE_NAME} ${SFML_DEPENDENCIES})
