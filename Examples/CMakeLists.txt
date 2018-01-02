project(TaskExamples)

set(ex1_srcs BasicExample.cpp)
add_executable(BasicExample ${ex1_srcs})
target_include_directories(BasicExample PRIVATE ${PROJECT_SOURCE_DIR}/Source)
target_include_directories(BasicExample PUBLIC Deps/fmt-4.0.0)
target_link_libraries(BasicExample JobRocket)
target_link_libraries(BasicExample fmt::fmt-header-only)

set(ex2_srcs MinimalExample.cpp)
add_executable(MinimalExample ${ex2_srcs})
target_include_directories(MinimalExample PRIVATE ${PROJECT_SOURCE_DIR}/Source)
target_link_libraries(MinimalExample JobRocket)
