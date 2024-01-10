cmake_minimum_required(VERSION 3.27)
project(VulkanApp)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

add_executable(VulkanApp src/main.cpp
        src/VulkanApp.cpp
        src/VulkanApp.h
        src/QueueFamilyIndices.h
        src/QueueFamilyIndices.cpp
        src/SwapChainSupportDetails.h
        src/SwapChainSupportDetails.cpp
        src/DebugUtils.cpp
        src/DebugUtils.h
)
target_link_libraries(VulkanApp Vulkan::Vulkan glfw fmt spdlog)

# enable link-time optimizations (-flto -flto -fno-fat-lto-objects for GCC)
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

if( supported )
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET VulkanApp PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()
