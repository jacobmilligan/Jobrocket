MESSAGE("-- Configuring and building 'fmt' library")

execute_process(COMMAND git submodule update --init fmt
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/fmt EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} fmt::fmt-header-only)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/fmt)
