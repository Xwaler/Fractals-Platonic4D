cmake_minimum_required(VERSION 3.14)
project(Fractals-Platonic4D)

set(CMAKE_CXX_STANDARD 14)
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libraries/glfw")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libraries/glm")

add_executable(${PROJECT_NAME} main.cpp shader.h tools.h sponge.h)

target_link_libraries(${PROJECT_NAME} glfw)

set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")
