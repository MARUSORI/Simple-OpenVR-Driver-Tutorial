cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp include/*.h)

add_library(01_basic_setup SHARED ${SOURCES};${HEADERS})
target_include_directories(01_basic_setup PRIVATE include/)

set_target_properties(01_basic_setup PROPERTIES PREFIX "driver_")

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

if(NOT DEFINED OPENVR_INCLUDE_DIR OR NOT DEFINED OPENVR_LIB_DIR)
    message(FATAL_ERROR "Please generate the project file using the CMakeLists file included in the root directory.")
endif()

target_include_directories(01_basic_setup PRIVATE ${OPENVR_INCLUDE_DIR})
target_link_libraries(01_basic_setup ${OPENVR_LIB_DIR}/openvr_api.lib)