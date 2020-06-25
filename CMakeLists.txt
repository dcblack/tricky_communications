# CMakeLists.txt -- Top-level CMake script

cmake_minimum_required( VERSION 3.11 )
project( mailbox CXX C )
set( TARGET_NAME mailbox )

set( CMAKE_C_STANDARD 11 CACHE STRING
     "C standard to build all targets." )

set( CMAKE_CXX_STANDARD 17 CACHE STRING
     "C++ standard to build all targets." )

set( CMAKE_CXX_STANDARD_REQUIRED 17 CACHE BOOL
     "CMAKE_CXX_STANDARD selected C++ standard is a requirement." )

set( default_build_type "Release" )

if( MSVC )
    add_compile_options( "/W4" "$<$<CONFIG:RELEASE>:/O2>" )
else()
    add_compile_options( "-Wall" "-Wextra" "-Werror" "$<$<CONFIG:RELEASE>:-O3>" )
    if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
        add_compile_options( "-stdlib=libc++" )
    else()
        # nothing special for gcc at the moment
    endif()
endif()

# C++
add_executable( testcpp testcpp.cpp )
target_compile_features( testcpp PUBLIC cxx_std_17 )

# C
add_executable( testc mailbox.c testc.cpp )
target_compile_features( testc PUBLIC cxx_std_17 )

# Test condition variables
set( TARGET_NAME cv EXCLUDE_FROM_ALL cv.cpp )
