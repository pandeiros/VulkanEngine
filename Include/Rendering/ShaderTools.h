/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Shaders.h"

#include <glslang/Include/ResourceLimits.h>

/**
 * @file ShaderTools.h
 */

VULKAN_NS_BEGIN

struct ShaderCache
{
    ShaderCache()
        : shaderHash(0), shaderType(VK_SHADER_STAGE_VERTEX_BIT)
    {}

    ShaderCache(size_t shaderHash)
        : shaderHash(shaderHash), shaderType(VK_SHADER_STAGE_VERTEX_BIT)
    {}

    ShaderCache(size_t shaderHash, VkShaderStageFlagBits shaderType, std::vector<unsigned int>& spirvData)
        : shaderHash(shaderHash), shaderType(shaderType), spirvData(spirvData)
    {}

    bool operator==(ShaderCache& other) const
    {
        return shaderHash == other.shaderHash;
    }

    size_t shaderHash;
    VkShaderStageFlagBits shaderType;
    std::vector<unsigned int> spirvData;
};

/**
 * @class ShaderTools
 */
class ShaderTools
{
public:
    static void InitGLSLang();
    static void FinalizeGLSLang();

    static void InitResources(TBuiltInResource &Resources);
    static bool glslToSPIRV(VkShaderStageFlagBits shaderStage, const char* shaderText, std::vector<unsigned int>& spirvData);

private:
#ifdef __ANDROID__
    static shaderc_shader_kind MapShadercType(VkShaderStageFlagBits vkShader);
#endif
};

VULKAN_NS_END