cmake_minimum_required(VERSION 3.7.1)
project(Jobrocket)
set(CMAKE_CXX_STANDARD 14)

include(${PROJECT_SOURCE_DIR}/CMake/jobrocket.cmake)

jobrocket_init()

add_subdirectory(Source)

if (BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif ()

if (BUILD_TESTS)
    add_subdirectory(Tests)
endif ()


add_library(${PROJECT_NAME} ${source_list})

if (NOT EXISTS ${PROJECT_SOURCE_DIR}/Deps/hwloc/install)
    MESSAGE("-- Configuring and building 'hwloc' library")
    file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/Deps/hwloc/install)
    execute_process ( COMMAND ./autogen.sh
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Deps/hwloc/ )
    execute_process ( COMMAND ./configure --prefix=${PROJECT_SOURCE_DIR}/Deps/hwloc/install --disable-shared --enable-static --disable-libxml2
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Deps/hwloc/ )
    execute_process ( COMMAND make
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Deps/hwloc/ )
    execute_process ( COMMAND make install
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Deps/hwloc/ )
endif ()

find_library(hwloc_lib hwloc ${PROJECT_SOURCE_DIR}/Deps/hwloc/install/lib)

if (hwloc_lib)
    target_link_libraries(${PROJECT_NAME} ${hwloc_lib})
endif ()

MESSAGE("-- Configuring and building 'fmt' library")
add_subdirectory(Deps/fmt EXCLUDE_FROM_ALL)


target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Source)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/Deps/hwloc/install/include)
target_include_directories(${PROJECT_NAME} PUBLIC Tests/catch)
target_include_directories(${PROJECT_NAME} PUBLIC Deps/fmt)

target_link_libraries(${PROJECT_NAME} fmt::fmt-header-only)
