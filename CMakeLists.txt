cmake_minimum_required(VERSION 3.7.1)

project(JobRocket)

set(CMAKE_CXX_STANDARD 14)

include(${PROJECT_SOURCE_DIR}/CMake/tasks.cmake)

enable_language(ASM)

add_subdirectory(Source)
add_subdirectory(Examples)
add_subdirectory(Tests)

add_library(${PROJECT_NAME} ${source_list})

find_library(hwloc_lib hwloc ${PROJECT_SOURCE_DIR}/Deps/hwloc/install/lib)

if (hwloc_lib)
    target_link_libraries(${PROJECT_NAME} ${hwloc_lib})
endif ()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(Deps/glfw-3.2.1)
add_subdirectory(Deps/fmt-4.0.0 EXCLUDE_FROM_ALL)

find_package(OpenGL REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Source)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Deps/hwloc/install/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Deps/ConcurrentQueue)
target_include_directories(${PROJECT_NAME} PUBLIC Tests/catch)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Deps/glfw-3.2.1/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC Deps/fmt-4.0.0)

target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} fmt::fmt-header-only)
