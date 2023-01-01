cmake_minimum_required (VERSION 3.8)

project ("ObjDebug")

aux_source_directory(. DIR_SRCS)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${DIR_SRCS} ${PROTO_SRCS} ${PROTO_HDRS})

add_definitions("-Wall -g")

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARIES})