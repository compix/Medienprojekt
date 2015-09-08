cmake_minimum_required (VERSION 2.6)

project(Game)

#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}/Game)
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Game)
#set(CMAKE_BUILD_TYPE Release)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
endif()

file(GLOB_RECURSE 
		BOMBAMAN_SOURCES
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h*)

set(EXECUTABLE_NAME "Game")
add_executable(${EXECUTABLE_NAME} Main.cpp ${BOMBAMAN_SOURCES})

# Create source groups for Visual Studio filters
# Get all directories first:
set(dir_list "")
foreach(file_path ${BOMBAMAN_SOURCES})
	get_filename_component(dir_path ${file_path} PATH)
	set(dir_list ${dir_list} ${dir_path})
endforeach()
list(REMOVE_DUPLICATES dir_list)

file(GLOB sources *.cpp *.h*)
source_group(\\ FILES ${sources})

foreach(dir ${dir_list})
	set(abs_dir "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
	file(GLOB sources ${abs_dir}/*.cpp ${abs_dir}/*.h*)
	string(REPLACE "/" "\\\\" Group ${dir})
	source_group(${Group} FILES ${sources})
endforeach()

add_definitions( -DSFML_STATIC )
add_definitions( -DHAS_STD_MAKE_UNIQUE )
#set(SFML_STATIC_LIBRARIES TRUE)

#find_package(SFML COMPONENTS graphics window system audio REQUIRED)
#find_package(TGUI REQUIRED)

#include_directories(${TGUI_INCLUDE_DIR})
#include_directories(${SFML_INCLUDE_DIR})

if(WIN32)
	# ws2_32 and winmm is needed for enet on windows
    set(ENET_LIBRARIES enet ws2_32 winmm)
else()
	set(ENET_LIBRARIES enet)
endif()

target_link_libraries(${EXECUTABLE_NAME} Box2D format ${ENET_LIBRARIES} entityx jsoncpp_lib_static sfml-system sfml-graphics sfml-window sfml-audio)

add_subdirectory(${ROOT_DIR}/ThirdParty/TGUI ${CMAKE_BINARY_DIR}/ThirdParty/TGUI)

target_link_libraries(${EXECUTABLE_NAME} tgui)
#target_link_libraries(${EXECUTABLE_NAME} ${SFML_DEPENDENCIES}) 