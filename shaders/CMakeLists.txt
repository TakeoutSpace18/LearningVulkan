cmake_minimum_required(VERSION 3.27)
project(shaders)

find_package(Vulkan REQUIRED COMPONENTS glslc)

set(SHADER_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(SHADER_BINARY_DIR ${PROJECT_BINARY_DIR})

file(GLOB_RECURSE SHADER_SOURCES
        ${SHADER_SOURCE_DIR}/*.frag
        ${SHADER_SOURCE_DIR}/*.vert
        ${SHADER_SOURCE_DIR}/*.comp
        ${SHADER_SOURCE_DIR}/*.geom
        ${SHADER_SOURCE_DIR}/*.tesc
        ${SHADER_SOURCE_DIR}/*.tese
        ${SHADER_SOURCE_DIR}/*.mesh
        ${SHADER_SOURCE_DIR}/*.task
        ${SHADER_SOURCE_DIR}/*.rgen
        ${SHADER_SOURCE_DIR}/*.rchit
        ${SHADER_SOURCE_DIR}/*.rmiss
)

foreach (src IN LISTS SHADER_SOURCES)
    get_filename_component(src_filename ${src} NAME)
    add_custom_command(
            COMMAND
            ${Vulkan_GLSLC_EXECUTABLE}
            -o ${SHADER_BINARY_DIR}/${src_filename}.spv
            ${src}
            DEPENDS ${src}
            OUTPUT ${SHADER_BINARY_DIR}/${src_filename}.spv
            COMMENT "Compiling ${src_filename}"
    )
    list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${src_filename}.spv)
endforeach ()

add_custom_target(CompileShaders ALL DEPENDS ${SPV_SHADERS})
