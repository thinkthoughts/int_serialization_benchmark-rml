cmake_minimum_required(VERSION 3.10)
project(BenchmarkProject VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD_23)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

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
CPMAddPackage(
  GITHUB_REPOSITORY jarro2783/cxxopts
  VERSION 3.2.0
  OPTIONS "CXXOPTS_BUILD_EXAMPLES NO" "CXXOPTS_BUILD_TESTS NO" "CXXOPTS_ENABLE_INSTALL YES"
)

include_directories(benchmarks src)

add_executable(benchmark
  benchmarks/benchmark.cpp
  src/third_party/itoa_an.c
  src/third_party/itoa_yy.c
)
target_compile_options(benchmark PRIVATE ${PROJECT_WARNINGS})
target_link_libraries(benchmark PRIVATE cxxopts)

if (NOT CYGWIN AND CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  set(ABSL_PROPAGATE_CXX_STD ON)
  set(ABSL_ENABLE_INSTALL OFF)
  set(ABSL_RUN_TEST OFF CACHE INTERNAL "")
  set(ABSL_USE_GOOGLETEST_HEAD OFF CACHE INTERNAL "")

  FetchContent_Declare(abseil
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG "d7aaad8"
    GIT_SHALLOW TRUE
  )
  FetchContent_GetProperties(abseil)
  if(NOT abseil_POPULATED)
    set(BUILD_TESTING OFF)
    FetchContent_MakeAvailable(abseil)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${abseil_SOURCE_DIR}/absl/copts)
    include(${abseil_SOURCE_DIR}/absl/copts/AbseilConfigureCopts.cmake)
  endif()

  target_link_libraries(benchmark PRIVATE absl::strings)
  target_compile_definitions(benchmark PRIVATE ABSEIL_SUPPORTED=1)
else()
  target_compile_definitions(benchmark PRIVATE ABSEIL_SUPPORTED=0)
endif()

option(BENCHMARK_SANITIZE "Sanitize memory" OFF)



add_executable(tester src/tester.cpp)
target_compile_options(tester PRIVATE ${PROJECT_WARNINGS})


if(BENCHMARK_SANITIZE)
  message(STATUS "Enabling AddressSanitizer")
  target_compile_options(benchmark PUBLIC -fsanitize=address  -fno-omit-frame-pointer -fno-sanitize-recover=all)
  target_compile_definitions(benchmark PUBLIC ASAN_OPTIONS=detect_leaks=1)
  target_link_libraries(benchmark PUBLIC -fsanitize=address  -fno-omit-frame-pointer -fno-sanitize-recover=all)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES AND NOT BENCHMARK_SANITIZE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
