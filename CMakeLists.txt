cmake_minimum_required(VERSION 3.7.1)
project(JobRocket)
set(CMAKE_CXX_STANDARD 14)

include(${PROJECT_SOURCE_DIR}/CMake/tasks.cmake)

add_subdirectory(Source)

if (WITH_EXAMPLES)
    add_subdirectory(Examples)
endif ()

if (WITH_TESTS)
    add_subdirectory(Tests)
endif ()

add_library(${PROJECT_NAME} ${source_list})

find_library(hwloc_lib hwloc ${PROJECT_SOURCE_DIR}/Deps/hwloc/install/lib)

if (hwloc_lib)
    target_link_libraries(${PROJECT_NAME} ${hwloc_lib})
endif ()

add_subdirectory(Deps/fmt-4.0.0 EXCLUDE_FROM_ALL)


target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Source)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Deps/hwloc/install/include)
target_include_directories(${PROJECT_NAME} PUBLIC Tests/catch)
target_include_directories(${PROJECT_NAME} PUBLIC Deps/fmt-4.0.0)

target_link_libraries(${PROJECT_NAME} fmt::fmt-header-only)
