list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")




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




# link with my own boost library in case we want thread sanitizers (this is necessary for boost::lockfree)
if("${FSAN}" STREQUAL "thread")
  set(Boost_NO_SYSTEM_PATHS ON)
  set(BOOST_ROOT "${BOOST160_TSAN_ROOT}")
  set(BOOST_LIBRARY_DIR "${BOOST160_TSAN_ROOT}/lib")
  find_package(Boost 1.60.0 COMPONENTS system regex coroutine context thread REQUIRED)
else()
  find_package(Boost 1.54.0 COMPONENTS system regex coroutine context thread REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIR})

if(APPLE)
  set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif()

#TODO: add requirement for version 1.0.1g (can it be done in one line?)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(Threads REQUIRED)

find_package(Protobuf REQUIRED)
 include_directories(${PROTOBUF_INCLUDE_DIRS})
 include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB PROTOFILES
  "proto/*.proto")

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS "${PROTOFILES}")

include_directories(.)
include_directories(include)
file(GLOB CG_SRC_FILES
  "*.cpp"
  "msg/*.cpp"
  "msg/*.cc"
  "*.hpp"
  "include/*.hpp")

# ODB
find_package(ODB REQUIRED COMPONENTS mysql)
include(${ODB_USE_FILE})

#set(CG_ODB_HEADERS
#"db/PlayerAccount.hpp")

file(GLOB CG_ODB_HEADERS
  "db/*.hpp")

include_directories(db)

#GET_DIRECTORY_PROPERTY(CG_ODB_INCLUDE_DIRS INCLUDE_DIRECTORIES)
set(CG_ODB_INCLUDE_DIRS
  /home/marius/cg/src
    /home/marius/cg/src/include
      /home/marius/cg/src/include/websock
      "${CMAKE_CURRENT_BINARY_DIR}")

set(ODB_COMPILE_DEBUG TRUE)
odb_compile(CG_SRC_FILES INCLUDE ${CG_ODB_INCLUDE_DIRS} FILES ${CG_ODB_HEADERS} DB mysql GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA SCHEMA_FORMAT embedded STANDARD c++11)


add_executable(cgserver 
${CG_SRC_FILES} 
${PROTO_SRCS} 
${PROTO_HDRS}
	${CG_ODB_HEADERS})

target_link_libraries(cgserver
	${ODB_LIBRARIES})

target_include_directories(cgserver
	PRIVATE
		${ODB_INCLUDE_DIRS}
		${ODB_COMPILE_OUTPUT_DIR})
target_compile_definitions(cgserver
	PRIVATE
		DATABASE_MYSQL)

if("${FSAN}" STREQUAL "thread")
  target_link_libraries(cgserver ${BOOST160_TSAN_ROOT}/lib/libboost_regex.a)
endif()

#target_link_libraries(cgserver libprotobuf.so)
target_link_libraries(cgserver ${PROTOBUF_LIBRARIES})
target_link_libraries(cgserver ${Boost_LIBRARIES})
target_link_libraries(cgserver ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(cgserver ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(cgserver PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})




if(MSYS)
    target_link_libraries(cgserver ws2_32 wsock32)
#    target_link_libraries(wss_examples ws2_32 wsock32)
endif()




