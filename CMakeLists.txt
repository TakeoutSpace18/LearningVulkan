cmake_minimum_required(VERSION 3.27)
project(VulkanApp)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(fmt REQUIRED)

add_executable(VulkanApp src/main.cpp
        src/VulkanApp.cpp
        src/VulkanApp.h
)
target_link_libraries(VulkanApp Vulkan::Vulkan glfw fmt)
