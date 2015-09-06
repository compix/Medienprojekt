cmake_minimum_required (VERSION 2.6)

# Ignore all warnings
set(CMAKE_CXX_FLAGS "-w")
set(ENTITYX_BUILD_SHARED FALSE)

add_subdirectory(Box2D)
add_subdirectory(cppformat) 
add_subdirectory(enet)
add_subdirectory(entityx)
add_subdirectory(jsoncpp)
add_subdirectory(SFML)
