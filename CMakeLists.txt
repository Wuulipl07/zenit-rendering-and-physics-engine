cmake_minimum_required(VERSION 3.16)
project(Zenit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # dla clangd / IDE

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

file(GLOB_RECURSE ZENIT_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(zenit ${ZENIT_SOURCES})
target_include_directories(zenit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(zenit PRIVATE OpenGL::GL glfw glm::glm)
target_compile_options(zenit PRIVATE -Wall -Wextra)
target_compile_definitions(zenit PRIVATE
  GLFW_INCLUDE_NONE
  ZENIT_ROOT="${CMAKE_SOURCE_DIR}")
