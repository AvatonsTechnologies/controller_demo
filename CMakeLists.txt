cmake_minimum_required(VERSION 2.8)
project(controller_demo)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include project files

add_subdirectory(src)

# Set output files

add_executable(demo ${DEMO_SOURCES} ${DEMO_HEADERS})
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

find_package(vjoy REQUIRED)
if (vjoy_FOUND)
    include_directories(demo ${vjoy_INCLUDE_DIRS})
    target_link_libraries(demo ${vjoy_LIBRARIES})
endif ()

# Configure CxxTest

set(CXXTEST_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cxxtest/test.cpp)

add_executable(all_tests ${TEST_SOURCES} ${TEST_HEADERS} ${CXXTEST_OUTPUT})

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/cxxtest)
find_package(CXXTEST REQUIRED)
if (CXXTEST_FOUND)
    include_directories(all_tests ${CXXTEST_INCLUDE_DIRS})
endif ()

add_custom_command(
    OUTPUT ${CXXTEST_OUTPUT}
    COMMAND ${CXXTEST_PYTHON_TESTGEN_EXECUTABLE} --error-printer -o ${CXXTEST_OUTPUT} ${TEST_DEFS}
    DEPENDS ${TEST_DEFS}
    )

enable_testing()
add_test(all_tests all_tests)
