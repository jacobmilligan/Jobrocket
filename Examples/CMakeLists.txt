project(TaskExamples)

set(ex1_srcs BasicExample.cpp)
add_executable(BasicExample ${ex1_srcs})
target_include_directories(BasicExample PRIVATE ${PROJECT_SOURCE_DIR}/Source)
target_link_libraries(BasicExample JobRocket)

set(ex2_srcs JobSchedulerExample.cpp)
add_executable(JobSchedulerExample ${ex2_srcs})
target_include_directories(JobSchedulerExample PRIVATE ${PROJECT_SOURCE_DIR}/Source)
target_link_libraries(JobSchedulerExample JobRocket)
