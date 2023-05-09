cmake_minimum_required(VERSION 3.25)
project(SCL)

set(CMAKE_CXX_STANDARD 20)

# Print build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("-- Build type is Debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("-- Build type is Release")
else()
    message("-- Build type is Unknown")
endif()

# Dependencies: OpenCV
find_package(OpenCV CONFIG REQUIRED)

# Add include directory
include_directories(include)

# Add source files
set(SOURCES
        src/ParamSet.cpp
        src/Simulator.cpp
)

# Copy resources in input/ to the binary directory
if(${CMAKE_GENERATOR} MATCHES "Visual Studio")
    # Visual Studio builds the executable into a separate directory
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        file(COPY ${CMAKE_SOURCE_DIR}/input DESTINATION ${CMAKE_BINARY_DIR}/Debug)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        file(COPY ${CMAKE_SOURCE_DIR}/input DESTINATION ${CMAKE_BINARY_DIR}/Release)
    endif()
else()
    file(COPY ${CMAKE_SOURCE_DIR}/input DESTINATION ${CMAKE_BINARY_DIR})
endif()

# Add target
add_executable(SCL main.cpp ${SOURCES})

# Linking
target_link_libraries(SCL PRIVATE opencv_core opencv_highgui opencv_imgcodecs)
