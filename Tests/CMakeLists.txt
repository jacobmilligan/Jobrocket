cmake_minimum_required(VERSION 3.7.1)

project(JobRocketTests)

set(CMAKE_CXX_STANDARD 14)

set(test_list "")

function(add_test test_name)
    set(srcs)
    foreach(s IN LISTS ARGN)
        if (NOT IS_ABSOLUTE "${s}")
            get_filename_component(s "${s}" ABSOLUTE)
        endif ()
        list(APPEND srcs "${s}")
    endforeach()
    add_executable(${test_name} "${srcs}" TestsMain.cpp)
    target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/Source)
    target_link_libraries(${test_name} JobRocket)
    list(APPEND test_list "${test_name}")
endfunction()


add_test(TestQueue TestQueue.cpp)
add_test(TestWorkers TestWorker.cpp)
add_test(TestPoolAllocator TestPoolAllocator.cpp)
add_test(TestJobPool TestJobPool.cpp)
