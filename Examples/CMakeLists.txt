project(TaskExamples)

set(ex1_srcs main.cpp)
add_executable(GeneralTest ${ex1_srcs})
target_include_directories(GeneralTest PRIVATE ${PROJECT_SOURCE_DIR}/Source)
target_link_libraries(GeneralTest JobRocket)

set(ex2_srcs TaskSchedulerExample.cpp)
add_executable(TaskSchedulerExample ${ex2_srcs})
target_include_directories(TaskSchedulerExample PRIVATE ${PROJECT_SOURCE_DIR}/Source)
target_link_libraries(TaskSchedulerExample JobRocket)