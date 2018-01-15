/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Shaders.h"

#include <shaderc/shaderc.h>
#include <glslang/Include/ResourceLimits.h>
//#include <glslang/Include/ResourceLimits.h>

/**
 * @file ShaderTools.h
 */

VULKAN_NS_BEGIN

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

    //static VkPipelineShaderStageCreateInfo
};

VULKAN_NS_END