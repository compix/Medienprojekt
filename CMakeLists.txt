cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(SFML_INCLUDE_DIR ThirdParty/SFML/include)
add_definitions(-DSFML_STATIC)
set(TGUI_SHARED_LIBS FALSE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wextra -Wall -pedantic -Wno-unused-parameter -Wno-unused-result")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

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
