cmake_minimum_required (VERSION 2.8)
project (CG C CXX)



set(SERVERAPP_SRC_DIR "${PROJECT_SOURCE_DIR}/serverapp/src")
set(SERVERAPP_PROTO_DIR "${PROJECT_SOURCE_DIR}/serverapp/proto")
set(HTML_PROTO_OUT_DIR "${PROJECT_SOURCE_DIR}/html/proto")

set(CG_ORIGIN "${PROJECT_SOURCE_DIR}")


# To compile with thread sanitizer, we need precompiled boost libraries (especially boost::lockfree) that
# have been instrumentalized with thread sanitizer (-fsanitize=thread).
# The following variable must be set to the boost root containing instrumented libraries of boost version 1.60
# Note that the program will only link against them if FSAN is set to 'thread'
set(BOOST160_TSAN_ROOT "${PROJECT_SOURCE_DIR}/boost-1.60-tsan")
set(BOOST160_ASAN_ROOT "${PROJECT_SOURCE_DIR}/boost-1.60-asan")

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  message("Note: No build type specified, choosing debug instead of release")
  set(CMAKE_BUILD_TYPE "debug")
  # set default sanitizer to address
  if("${FSAN}" STREQUAL "")
	    message("No sanitizer specified, choosing default.")
    set(FSAN "address")
  endif()
endif()
message("Selecting sanitizer: ${FSAN}")

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
set(ODB_SHUT_UP_FLAG "-Wno-unknown-pragmas")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -fsanitize=${FSAN}  ${ODB_SHUT_UP_FLAG}")

add_definitions(-std=c++14)
configure_file(
    "${PROJECT_SOURCE_DIR}/filter/gcc_filter.sh.in"
   "${PROJECT_BINARY_DIR}/gcc_filter.sh"
    @ONLY)
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${PROJECT_BINARY_DIR}/gcc_filter.sh")


add_subdirectory(src)
