cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp include/*.h)

add_library(02_trackers SHARED ${SOURCES};${HEADERS})
target_include_directories(02_trackers PRIVATE include/)

set_target_properties(02_trackers PROPERTIES PREFIX "driver_")

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

if(NOT DEFINED OPENVR_INCLUDE_DIR OR NOT DEFINED OPENVR_LIB_DIR)
    message(FATAL_ERROR "Please generate the project file using the CMakeLists file included in the root directory.")
endif()

target_include_directories(02_trackers PRIVATE ${OPENVR_INCLUDE_DIR})
target_link_libraries(02_trackers ${OPENVR_LIB_DIR}/openvr_api.lib)