cmake_minimum_required (VERSION 2.6)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(SFML_INCLUDE_DIR ThirdParty/SFML/include)
add_definitions(-DSFML_STATIC)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Game)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Temp)

set(GAME_DIR ${ROOT_DIR}/Game)
file(GLOB GAME_RESOURCES "${GAME_DIR}/*")

if(MSVC)
    # Handle Visual Studio Debug/Release... config folders
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/Game)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/lib)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/lib)
        set(PDB_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/Temp)
    endforeach()

    # Copy assets to the binary directory
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        file(COPY ${GAME_RESOURCES} DESTINATION ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/Game)
    endforeach()
else()
    # Copy assets to the binary directory
    file(COPY ${GAME_RESOURCES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall -pedantic -Wno-unused-parameter -Wno-unused-result -pthread")
endif()

include_directories(${PROJECT_BINARY_DIR})
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})

include_directories(ThirdParty/Box2D
                    ThirdParty/cppformat
                    ThirdParty/enet/include
                    ThirdParty/entityx
                    ThirdParty/jsoncpp/include
	                ${SFML_INCLUDE_DIR}
                    ThirdParty/TGUI/include)

add_subdirectory(ThirdParty)
add_subdirectory(Source)
