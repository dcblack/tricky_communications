# CMakeLists.txt -- Top-level CMake script

cmake_minimum_required( VERSION 3.11 )
project( mailbox CXX C )
set( TARGET_NAME mailbox )

set( CMAKE_CXX_STANDARD 17 CACHE STRING
     "C++ standard to build all targets." )

set( CMAKE_CXX_STANDARD_REQUIRED 17 CACHE BOOL
     "CMAKE_CXX_STANDARD selected C++ standard is a requirement." )

set( default_build_type "Release" )

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if( MSVC )
    add_compile_options( "/W4" "$<$<CONFIG:RELEASE>:/O2>" )
else()
    add_compile_options( "-Wall" "-Wextra" "-Werror" "$<$<CONFIG:RELEASE>:-O3>" )
endif()

# C++
add_executable( testcpp testcpp.cpp )
target_compile_features( testcpp PUBLIC cxx_std_17 )
target_link_libraries( testcpp PRIVATE Threads::Threads)

# C
add_executable( testc mailbox.c testc.cpp )
target_compile_features( testc PUBLIC cxx_std_17 )

# Test condition variables
set( TARGET_NAME cv EXCLUDE_FROM_ALL cv.cpp )
