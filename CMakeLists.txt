cmake_minimum_required(VERSION 3.8)

project(AxMarshal VERSION 0.1.0)

aux_source_directory(. DIR_SRCS)
aux_source_directory(CodeGenerator DIR_SRCS)

add_executable(${PROJECT_NAME} ${DIR_SRCS})

add_compile_options(-Wall)

# set(THREADS_PREFER_PTHREAD_FLAG ON) find_package(Threads)
# target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

find_package(fmt)
target_link_libraries(${PROJECT_NAME} fmt)

add_subdirectory(File)
target_link_libraries(${PROJECT_NAME} File)

add_subdirectory(FrontEnd)
target_link_libraries(${PROJECT_NAME} FrontEnd)

add_subdirectory(BackEnd)
target_link_libraries(${PROJECT_NAME} BackEnd)
