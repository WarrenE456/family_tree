cmake_minimum_required(VERSION 3.22.1)
project(tree)

file(GLOB SRC_FILES src/*)
add_executable(${PROJECT_NAME} ${SRC_FILES})
