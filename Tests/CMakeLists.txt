cmake_minimum_required(VERSION 3.1.0)
project(JobrocketTests)

# Adds a new test executable with all
function(add_test test_name)
    set(srcs)
    foreach(s IN LISTS ARGN)
        if (NOT IS_ABSOLUTE "${s}")
            get_filename_component(s "${s}" ABSOLUTE)
        endif ()
        list(APPEND srcs "${s}")
    endforeach()
    add_executable(${test_name} "${srcs}")
    target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/../Source)
    target_link_libraries(${test_name} Jobrocket)
    target_link_libraries(${test_name} TestsMain)
    list(APPEND test_list "${test_name}")
endfunction()

########################
## Tests configurations
########################

set(CMAKE_CXX_STANDARD 14)

set(test_list "")

add_library(TestsMain STATIC TestsMain.cpp)

add_test(TestQueue TestQueue.cpp)
add_test(TestWorkers TestWorker.cpp)
add_test(TestPoolAllocator TestPoolAllocator.cpp)
add_test(TestJobPool TestJobPool.cpp)
