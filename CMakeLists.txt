cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


include(cmake/CPM.cmake)
CPMAddPackage("gh:fmtlib/fmt#10.2.1")


project(BenchmarkProject VERSION 1.0)


include_directories(benchmarks src)


add_executable(benchmark benchmarks/benchmark.cpp)
target_link_libraries(benchmark PRIVATE fmt::fmt)

add_executable(tester src/tester.cpp)
target_link_libraries(tester PRIVATE fmt::fmt)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

