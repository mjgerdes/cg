cmake_minimum_required (VERSION 2.8)
project (CardGameServer C CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -g -fsanitize=thread ") 
add_definitions(-std=c++14)
#set(CMAKE_CXX_COMPILER /usr/bin/g++-5)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_CXX_COMPILER clang++)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")# -fsanitize=thread -v")

configure_file(
    "${PROJECT_SOURCE_DIR}/gcc_filter.sh.in"
    "${PROJECT_BINARY_DIR}/gcc_filter.sh"
    @ONLY)




if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC version >=4.8 required.")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4)
        message(FATAL_ERROR "Clang version >=3.4 required.")
    endif()
else()
    message(WARNING "Your compiler (${CMAKE_CXX_COMPILER_ID}) has not been tested on this project. Only Clang and GCC has been tested. Please report any problems at the project page on GitHub.")
endif()

set(Boost_NO_SYSTEM_PATHS ON)
set(BOOST_ROOT "/home/marius/myboost2")
set(BOOST_LIBRARY_DIR /home/marius/myboost2/lib)
#Only tested with versions 1.55 and 1.56
#find_package(Boost 1.54.0 COMPONENTS system regex coroutine context thread REQUIRED)
find_package(Boost 1.60.0 COMPONENTS system regex coroutine context thread REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

if(APPLE)
  set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif()

#TODO: add requirement for version 1.0.1g (can it be done in one line?)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(Threads REQUIRED)

include_directories(.)

include_directories(include)
include_directories(src)


add_executable(cgserver src/main.cpp src/GameServer.cpp src/PlayerAccount.cpp src/LogServer.cpp src/msg/ClientMessage.pb.cc src/msg/Login.pb.cc)
#target_link_libraries(cgserver /home/marius/myboost2/libboost_regex.so.1.60.0)
target_link_libraries(cgserver /home/marius/myboost2/lib/libboost_regex.a)
target_link_libraries(cgserver libprotobuf.so)
target_link_libraries(cgserver ${Boost_LIBRARIES})
target_link_libraries(cgserver ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(cgserver ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(cgserver PROPERTIES RUNTIME_OUTPUT_DIRECTORY ./bin)


if(MSYS)
    target_link_libraries(cgserver ws2_32 wsock32)
#    target_link_libraries(wss_examples ws2_32 wsock32)
endif()

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${PROJECT_BINARY_DIR}/gcc_filter.sh")


