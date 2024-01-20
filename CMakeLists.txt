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
        src/utility/NonMovable.h
)
target_link_libraries(VulkanApp Vulkan::Vulkan glfw fmt spdlog)
target_include_directories(VulkanApp PRIVATE src)

target_precompile_headers(VulkanApp PRIVATE
        # containers
        <set>
        <vector>
        <map>
        <unordered_map>
        <string>

        # other stl stuff
        <memory>
        <functional>
        <iostream>
        <ios>
        <fstream>
        <optional>
        <limits>
        <algorithm>
        <cstdint>

        # logging
        <spdlog/spdlog.h>
)

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CMAKE_COMMAND} -E time")

target_compile_options(VulkanApp PRIVATE -Winvalid-pch)

# enable link-time optimizations in release builds (-flto -fno-fat-lto-objects for GCC)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)
if (ipo_supported)
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET VulkanApp PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    set_property(TARGET VulkanApp PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
    set_property(TARGET VulkanApp PROPERTY INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE)
    set_property(TARGET VulkanApp PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
else ()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif ()