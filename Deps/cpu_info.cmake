MESSAGE("-- Configuring and adding 'cpu_info' library")

execute_process(COMMAND git submodule update --init cpu_info
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/cpu_info/include)