cmake_minimum_required(VERSION 3.8)

project(FrontEnd)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_NAME}_SRCS)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})
add_compile_options(-Wall)
