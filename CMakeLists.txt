cmake_minimum_required(VERSION 3.27)
project(LearningVulkan)

set(CMAKE_CXX_STANDARD 20)

# use compiler cache
include(cmake/ccache.cmake)

add_subdirectory(shaders)

file(GLOB_RECURSE VULKANAPP_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(VulkanApp ${VULKANAPP_SOURCES})
add_dependencies(VulkanApp CompileShaders)
target_include_directories(VulkanApp PRIVATE src)

# dependencies
set(THIRDPARTY_DIR third-party)

# glfw
add_subdirectory(${THIRDPARTY_DIR}/glfw)
target_link_libraries(VulkanApp PRIVATE glfw)

# spdlog
add_subdirectory(${THIRDPARTY_DIR}/spdlog)
target_link_libraries(VulkanApp PRIVATE spdlog::spdlog)

# glm
add_subdirectory(${THIRDPARTY_DIR}/glm)
target_link_libraries(VulkanApp PRIVATE glm::glm)

# Vulkan
find_package(Vulkan REQUIRED COMPONENTS glslc)
target_link_libraries(VulkanApp PRIVATE Vulkan::Vulkan)
target_compile_definitions(VulkanApp
        PRIVATE
        VULKAN_HPP_NO_CONSTRUCTORS # in order to use designated initializers
)

target_precompile_headers(VulkanApp PRIVATE
        # containers
        <set>
        <vector>
        <array>
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
        <cstddef>

        # logging
        <spdlog/spdlog.h>

        # vulkan
        <vulkan/vulkan.hpp>
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
