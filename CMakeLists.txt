cmake_minimum_required(VERSION 3.27)
project(VulkanApp)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

add_executable(VulkanApp src/main.cpp
        src/vulkan/VulkanContext.cpp
        src/vulkan/VulkanContext.h
        src/vulkan/QueueFamilyIndices.h
        src/vulkan/QueueFamilyIndices.cpp
        src/vulkan/SwapChainSupportDetails.h
        src/vulkan/SwapChainSupportDetails.cpp
        src/vulkan/DebugUtils.cpp
        src/vulkan/DebugUtils.h
        src/vulkan/RenderPipeline.cpp
        src/vulkan/RenderPipeline.h
        src/Application.cpp
        src/Application.h
        src/glfw/GLFWContext.cpp
        src/glfw/GLFWContext.h
        src/utility/Utility.h
        src/utility/NonCopyable.h
        src/utility/NonMoveable.h
)
target_link_libraries(VulkanApp Vulkan::Vulkan glfw fmt spdlog)
target_include_directories(VulkanApp PRIVATE src)

target_compile_options(VulkanApp PRIVATE -Winvalid-pch)

# enable link-time optimizations (-flto -fno-fat-lto-objects for GCC)
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

if( supported )
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET VulkanApp PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()
