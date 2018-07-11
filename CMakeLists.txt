cmake_minimum_required(VERSION 3.3)

project(Iona-1)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

file(GLOB FSRC
    "src/Window/*.cpp"
    "src/Graphics/Vulkan/*.cpp"
    "src/Graphics/*.cpp"
    "src/Exception/*.cpp"
    "src/Generic/*.cpp"
    "src/Graphics/Vulkan/Helpers/*.cpp"
    "*.cpp"
)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/inc/)

find_library(glfw REQUIRED)
find_library(vulkan REQUIRED)
find_library(stdc++fs REQUIRED)
find_library(dl REQUIRED)

add_executable(iona ${FSRC})
target_link_libraries(iona dl glfw stdc++fs vulkan)
target_compile_definitions(iona PRIVATE GLFW_INCLUDE_NONE=1)