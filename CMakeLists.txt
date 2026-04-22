cmake_minimum_required(VERSION 3.25)
project(chemish LANGUAGES CXX)
 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED CONFIG)
 
add_executable(chemish src/main.cpp)
target_link_libraries(chemish PRIVATE Vulkan::Vulkan SDL3::SDL3)
target_compile_options(chemish PRIVATE -Wall -Wextra)
 

