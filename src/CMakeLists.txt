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
  find_package(Boost 1.60.0 COMPONENTS system regex coroutine context thread filesystem REQUIRED)
elseif("${FSAN}" STREQUAL "address")
  set(Boost_NO_SYSTEM_PATHS ON)
  set(BOOST_ROOT "${BOOST160_ASAN_ROOT}")
  set(BOOST_LIBRARY_DIR "${BOOST160_ASAN_ROOT}/lib")
  find_package(Boost 1.60.0 COMPONENTS system regex coroutine context thread filesystem REQUIRED)
else()
  find_package(Boost 1.54.0 COMPONENTS system regex coroutine context thread filesystem REQUIRED)
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
set(PROTOBUF_IMPORT_DIRS "${SERVERAPP_PROTO_DIR}")
file(GLOB PROTOFILES
  "proto/*.proto"
  "${SERVERAPP_PROTO_DIR}/*.proto")
message("PROTO_HDRS: ${PROTO_HDRS}")
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS "${PROTOFILES}")

include_directories(.)
include_directories(include)
include_directories("${SERVERAPP_SRC_DIR}")
file(GLOB CG_SRC_FILES
  "*.cpp"
  "msg/*.cpp"
  "GameServer/*.cpp"
  "Module/*.cpp"
  "msg/*.cc"
  "*.hpp"
  "include/*.hpp"
  "${SERVERAPP_SRC_DIR}/*.cpp"
  "${SERVERAPP_SRC_DIR}/*.hpp"
  "${SERVERAPP_SRC_DIR}/Module/*.cpp"
  "${SERVERAPP_SRC_DIR}/db/*.cpp")

# ODB
find_package(ODB REQUIRED COMPONENTS mysql)
include(${ODB_USE_FILE})

#set(CG_ODB_HEADERS
#"db/PlayerAccount.hpp"
#"${SERVERAPP_SRC_DIR}/db/Player.hpp")

file(GLOB CG_ODB_HEADERS
  "db/*.hpp"
  "${SERVERAPP_SRC_DIR}/db/*.hpp")

include_directories(db)
include_directories("${SERVERAPP_SRC_DIR}/db")
#GET_DIRECTORY_PROPERTY(CG_ODB_INCLUDE_DIRS INCLUDE_DIRECTORIES)
message("Origin: ${CG_ORIGIN}")
set(CG_ODB_INCLUDE_DIRS
#  /home/marius/cg/src
#    /home/marius/cg/src/include
#      /home/marius/cg/src/include/websock
  "${CG_ORIGIN}/src"
  "${CG_ORIGIN}/src/db"
    "${CG_ORIGIN}/src/include"
	  "${CG_ORIGIN}/src/include/websock"
	  "${SERVERAPP_SRC_DIR}"
      "${CMAKE_CURRENT_BINARY_DIR}"
	        "${CMAKE_CURRENT_BINARY_DIR}/src/odb_gen"
	  "${SERVERAPP_SRC_DIR}")

include_directories(	        "${CMAKE_CURRENT_BINARY_DIR}/src/odb_gen")
include_directories(	        "${CMAKE_CURRENT_BINARY_DIR}/odb_gen")
set(ODB_COMPILE_DEBUG TRUE)
odb_compile(CG_SRC_FILES INCLUDE ${CG_ODB_INCLUDE_DIRS} FILES ${CG_ODB_HEADERS} DB mysql GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA SCHEMA_FORMAT embedded STANDARD c++11)

message("Files: ${CG_SRC_FILES}")
message("ODB HEADERS: ${CG_ODB_HEADERS}")
message("ODB INCLUDE DIRS: ${CG_ODB_INCLUDE_DIRS}")
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

if("${FSAN}" STREQUAL "address")
  target_link_libraries(cgserver ${BOOST160_ASAN_ROOT}/lib/libboost_regex.a)
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

add_custom_command(TARGET cgserver PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/proto ${HTML_PROTO_OUT_DIR})

add_custom_command(TARGET cgserver PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${SERVERAPP_PROTO_DIR} ${HTML_PROTO_OUT_DIR})


  



