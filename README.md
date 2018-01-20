# Jobrocket

Jobrocket is a task-based parallelism library initially developed as an internal library for the [Skyrocket](https://github.com/jacobmilligan/Skyrocket) framework. It's designed to be lightweight with a simple interface, making use of C++11/14 concurrency primitives. Internally, Jobrocket uses a lock-free work-stealing algorithm to schedule jobs to workers.

[Documentation is available here](https://jacobmilligan.github.io/Jobrocket/).

## Example

Here's a quick example of how to use Jobrocket:

```cpp
#include <JobRocket/JobRocket.hpp>
#include <JobRocket/JobGroup.hpp>

int main()
{
    // Launch the scheduler with the desired number of workers and main-threads.
    // This example leaves the number of workers and main threads up to the scheduler which will
    // usually be the physical cores + 1 workers and 1 main thread
    jobrocket::startup(jobrocket::Scheduler::auto_thread_count,
                       jobrocket::Scheduler::auto_thread_count);

    // Allocate a few jobs using the job pool but don't run them
    auto* job1 = jobrocket::make_job([]() {
        printf("Hello from job 1\n");
    });

    auto* job2 = jobrocket::make_job([]() {
        printf("Hello from job 2\n");
    });

    // Allocate a job, run it, and then wait for the result
    jobrocket::make_job_and_wait([]() {
        printf("Hello from job 3\n");
    });

    // Run the earlier-allocated jobs
    jobrocket::run(job1);
    jobrocket::run(job2);

    // Wait for both to finish
    jobrocket::wait(job2);
    jobrocket::wait(job1);

    // JobRocket provides the `JobGroup` class to run many related jobs in parallel and wait on
    // all of them to finish
    jobrocket::JobGroup group;

    constexpr int num_jobs = 200;
    constexpr int expected_result = (num_jobs * (num_jobs + 1)) / 2;
    int value = 0;

    printf("\nJobGroup: Sum of 1..%d:\n", num_jobs);

    for ( int i = 1; i <= num_jobs; ++i ) {
        group.run([&, i]() {
            value += i;
        });
    }

    // Wait for all jobs to finish
    group.wait_for_all();

    printf("Expected value: %d | Final value: %d\n", expected_result, value);

    jobrocket::shutdown();
}
```

This should output something along the lines of:

```shell
Hello from job 3
Hello from job 1
Hello from job 2

JobGroup: Sum of 1..200:
Expected value: 20100 | Final value: 20100
```

## Getting Started

### Dependencies:

* CMake
* A compiler with minimum C++14 support
* OpenGL 4.0+ (for examples)

### Building:

`cd` into the Jobrocket root directory and run the following commands:

```shell
# setup directories
mkdir Build

# fetch and download dependencies with git submodule
cd Build

# make a directory for whatever configuration is going to be built (Debug/Release/RelWithDebInfo)
mkdir <build type>
cd <build type>

# configure CMake files.
# If running for the first time, all dependencies (git submodule) will be initialized and downloaded
cmake ../.. -DCMAKE_BUILD_TYPE=<build type>

# Build the project
cmake --build .
```

The following options are available with the CMake build by adding `-D<option>=on` after `cmake ..`

|Option|Description|
|------|-----------|
|`BUILD_EXAMPLES`|Builds all example programs along with the library into `Build/<BUILD_TYPE>/Examples`|
|`BUILD_TESTS`|Builds all unit tests along with the library|
|`CMAKE_BUILD_TYPE`|Specifies the build type for the project with the following available: `Debug`, `Release`, `RelWithDebInfo`|

## Usage
To use Jobrocket in your project first link the static library built in `/Build/` then add `Jobrocket_Root/Source` to your projects include path.

To use the library and simple tasks, simply include `<Jobrocket/Jobrocket.hpp>`. To use `JobGroup` objects, include `<Jobrocket/JobGroup>`.

## License

Jobrocket is available under the [MIT License](https://github.com/jacobmilligan/Jobrocket/blob/master/LICENSE).
