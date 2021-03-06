cmake_minimum_required(VERSION 3.14)
project(Fractals-Platonic4D)

set(CMAKE_CXX_STANDARD 14)
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libraries/glfw")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libraries/glm")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(GLM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/libraries/glm")

include_directories(${GLM_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} src/main.cpp src/vectorTools.cpp headers/vectorTools.h
        src/Sponge.cpp headers/Sponge.h src/Window.cpp headers/Window.h headers/Faces.h src/Menu.cpp headers/Menu.h headers/font.h headers/MenuProperties.h headers/Hypercube.h)

target_link_libraries(${PROJECT_NAME} glfw)

set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")
