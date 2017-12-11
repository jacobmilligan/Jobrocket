project(TaskExamples)

#set(example_sources ${example_sources} main.cpp)
#add_executable(${PROJECT_NAME} ${example_sources})
#target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/Source)
#target_link_libraries(${PROJECT_NAME} Tasks)

set(ex2_srcs TaskSchedulerExample.cpp)
add_executable(TaskSchedulerExample ${ex2_srcs})
target_include_directories(TaskSchedulerExample PRIVATE ${PROJECT_SOURCE_DIR}/Source)
target_link_libraries(TaskSchedulerExample Tasks)