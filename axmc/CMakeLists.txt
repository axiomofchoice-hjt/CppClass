cmake_minimum_required(VERSION 3.8)

project(axmc)

aux_source_directory(. ${PROJECT_NAME}_SRCS)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})

add_compile_options(-Wall)

add_subdirectory(File)
target_link_libraries(${PROJECT_NAME} File)

add_subdirectory(FrontEnd)
target_link_libraries(${PROJECT_NAME} FrontEnd)

add_subdirectory(BackEnd)
target_link_libraries(${PROJECT_NAME} BackEnd)
