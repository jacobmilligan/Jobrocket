cmake_minimum_required(VERSION 3.1.0)
project(Jobrocket)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
        ${PROJECT_SOURCE_DIR}/CMake
        ${PROJECT_SOURCE_DIR}/CMake/cmake_utils)

include(${PROJECT_SOURCE_DIR}/CMake/cmake_utils/cmake_utils.cmake)

cmake_utils_init()
add_subdirectory(Source)
add_library(${PROJECT_NAME} ${cu_source_list})

add_subdirectory(Deps)

cu_dump_includes()

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Source)
target_include_directories(${PROJECT_NAME} PUBLIC Tests/catch)

if (BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif ()

if (BUILD_TESTS)
    add_subdirectory(Tests)
endif ()
