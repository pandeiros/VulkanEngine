/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#ifdef __ANDROID__
#include "shaderc/shaderc.hpp"
#else
#include <shaderc/shaderc.hpp>
#endif

/**
 * @file Shaders.h
 */

VULKAN_NS_BEGIN

static const char* VULKAN_VERTEX_SHADER_TEXT =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (std140, binding = 0) uniform uniformValues {\n"
    "    mat4 modelViewMatrix;\n"
    "    mat4 projectionMatrix;\n"
    "    vec4 undistortion;\n"
    "} myUniformValues;\n"
    "layout (location = 0) in vec4 pos;\n"
    "layout (location = 1) in vec4 inColor;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "vec4 undistort(vec4 pos) {\n"
    "    float maxRadiusSquared = 100.0;\n"
    "    pos = myUniformValues.modelViewMatrix * pos;\n"
    "    float r2 = clamp(dot(pos.xy, pos.xy) / (pos.z * pos.z), 0, maxRadiusSquared);\n"
    "    pos.xy *= 1 + (myUniformValues.undistortion.x + myUniformValues.undistortion.y * r2) * r2;\n"
    "    return pos;\n"
    "}\n"
    "void main() {\n"
    "   outColor = inColor;\n"
    "   gl_Position = myUniformValues.projectionMatrix * undistort(pos);\n"
    "}\n";

static const char* VULKAN_FRAGMENT_SHADER_TEXT =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (location = 0) in vec4 color;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "void main() {\n"
    "   outColor = color;\n"
    "}\n";

struct ShaderEntry
{
    std::vector<const char*> vertexShaders;
    std::vector<const char*> fragmentShaders;
    std::vector<const char*> tesselationControlShaders;
    std::vector<const char*> tesselationEvaluationShaders;
};

// #REFACTOR
#ifdef __ANDROID__
struct shader_type_mapping {
    VkShaderStageFlagBits vkshader_type;
    shaderc_shader_kind shaderc_type;
};
static const shader_type_mapping shader_map_table[] = {
    {VK_SHADER_STAGE_VERTEX_BIT, shaderc_glsl_vertex_shader},
    {VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, shaderc_glsl_tess_control_shader},
    {VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, shaderc_glsl_tess_evaluation_shader},
    {VK_SHADER_STAGE_GEOMETRY_BIT, shaderc_glsl_geometry_shader},
    {VK_SHADER_STAGE_FRAGMENT_BIT, shaderc_glsl_fragment_shader},
    {VK_SHADER_STAGE_COMPUTE_BIT, shaderc_glsl_compute_shader},
};
#endif

VULKAN_NS_END