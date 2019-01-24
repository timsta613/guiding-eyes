#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)

project(GuidingEyesCameraMotor)

# Save the command line compile commands in the build output
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()


find_library(WIRINGPI_FOUND wiringPi HINTS ${LIBRARY_DIR} REQUIRED)
find_library(CRYPT_FOUND crypt)
find_package(OpenCV REQUIRED)
if(NOT WIRINGPI_FOUND)
    SET(WIRINGPI_FOUND "wiringPi")
    message(WARN "Failed to find_library(wiringPi)")
endif()

add_executable(camera-motor camera-motor.cpp ../example.hpp ../../third-party/imgui/imgui.cpp ../../third-party/imgui/imgui_draw.cpp ../../third-party/imgui/imgui_impl_glfw.cpp)
include_directories(camera-motor ${ADDITIONAL_INCLUDE_DIRS})
target_link_libraries(camera-motor ${DEPENDENCIES})
target_link_libraries(camera-motor ${REALSENSE2_FOUND})
target_link_libraries(camera-motor ${WIRINGPI_FOUND})
target_link_libraries(camera-motor ${CRYPT_FOUND})
target_link_libraries(camera-motor ${OpenCV_LIBS})

include_directories(camera-motor ../ ../../third-party/tclap/include ../../third-party/imgui)
set_target_properties (camera-motor PROPERTIES
    FOLDER "Examples"
)

install(
    TARGETS

    camera-motor

    RUNTIME DESTINATION
    ${CMAKE_INSTALL_PREFIX}/bin
)
