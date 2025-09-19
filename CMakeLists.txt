cmake_minimum_required(VERSION 3.10)
project(BenchmarkProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

function(check_avx512_support result_var)
  if(UNIX AND NOT APPLE) # Linux
    file(READ "/proc/cpuinfo" CPUINFO)
    string(FIND "${CPUINFO}" "avx512" AVX512_FOUND)
    if(NOT AVX512_FOUND EQUAL -1)
      set(${result_var} TRUE PARENT_SCOPE)
    else()
      set(${result_var} FALSE PARENT_SCOPE)
    endif()
  else() # AVX512 never available on apple computers
    set(${result_var} FALSE PARENT_SCOPE)
  endif()
endfunction()

if(MSVC)
  # On Windows, always use AVX512 flag
  set(SFF_MARCH_FLAG "/arch:AVX512")
  set(PROJECT_WARNINGS /W4)
else()
  # On Linux with GCC/Clang
  check_avx512_support(HOST_HAS_AVX512)
  if(HOST_HAS_AVX512)
    set(SFF_MARCH_FLAG "-march=native")
  else() # still enable baseline AVX512 support (e.g., for compile test on Github CI)
    set(SFF_MARCH_FLAG "-march=x86-64-v4")
  endif()

  set(PROJECT_WARNINGS
    -Wall
    -Wextra
  )
endif()

include(CheckCXXCompilerFlag)
if(SFF_MARCH_FLAG)
    check_cxx_compiler_flag(${SFF_MARCH_FLAG} COMPILER_SUPPORTS_ARCH_FLAG)
    if(COMPILER_SUPPORTS_ARCH_FLAG)
        message(STATUS "Compiler supports ${SFF_MARCH_FLAG}, enabling it.")
        add_compile_options(${SFF_MARCH_FLAG})
    endif()
else()
    message(STATUS "No AVX-512 flags applied (Apple or unsupported platform)")
endif()

include(cmake/CPM.cmake)
CPMAddPackage("gh:fmtlib/fmt#10.2.1")


include_directories(benchmarks src)

add_executable(benchmark benchmarks/benchmark.cpp)
target_compile_options(benchmark PRIVATE ${PROJECT_WARNINGS})
target_link_libraries(benchmark PRIVATE fmt::fmt)

add_executable(tester src/tester.cpp)
target_compile_options(tester PRIVATE ${PROJECT_WARNINGS})
target_link_libraries(tester PRIVATE fmt::fmt)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
