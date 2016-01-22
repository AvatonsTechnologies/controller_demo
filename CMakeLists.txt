cmake_minimum_required(VERSION 2.8)
project(controller_demo)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include project files

add_subdirectory(src)

# Set output files

add_executable(demo ${DEMO_SOURCES} ${DEMO_SOURCES})
add_executable(visualizer ${VISUALIZER_SOURCES} ${VISUALIZER_HEADERS})

# Find and include external libraries

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Tactonic REQUIRED)
if (Tactonic_FOUND)
    include_directories(demo ${Tactonic_INCLUDE_DIRS})
    target_link_libraries(demo ${Tactonic_LIBRARIES})
    include_directories(visualizer ${Tactonic_INCLUDE_DIRS})
    target_link_libraries(visualizer ${Tactonic_LIBRARIES})
endif ()
