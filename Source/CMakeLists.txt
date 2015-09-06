cmake_minimum_required (VERSION 2.6)

project(Game)

if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug")
endif()

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}/Game)
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Game)
#set(CMAKE_BUILD_TYPE Release)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
endif()

file(GLOB_RECURSE BOMBAMAN_SOURCES
         RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
                 *.cpp
                 *.h*)

set(EXECUTABLE_NAME "Game")
add_executable(${EXECUTABLE_NAME} Main.cpp ${BOMBAMAN_SOURCES})

add_definitions( -DSFML_STATIC )
add_definitions( -DHAS_STD_MAKE_UNIQUE )
#set(SFML_STATIC_LIBRARIES TRUE)

#find_package(SFML COMPONENTS graphics window system audio REQUIRED)
#find_package(TGUI REQUIRED)

#include_directories(${TGUI_INCLUDE_DIR})
#include_directories(${SFML_INCLUDE_DIR})

target_link_libraries(${EXECUTABLE_NAME} Box2D format enet entityx jsoncpp_lib_static sfml-system sfml-graphics sfml-window sfml-audio)

add_subdirectory(${ROOT_DIR}/ThirdParty/TGUI ${CMAKE_BINARY_DIR}/ThirdParty/TGUI)

target_link_libraries(${EXECUTABLE_NAME} tgui)
#target_link_libraries(${EXECUTABLE_NAME} ${SFML_DEPENDENCIES}) 
