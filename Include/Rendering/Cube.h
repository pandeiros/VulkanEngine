/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Shaders.h"
#include "RenderComponent.h"

/**
 * @file Cube.h
 */

VULKAN_NS_BEGIN

/**
 * @class Cube
 */
class Cube : public RenderComponent
{
public:
    static VertexData GetCubeVertexData();
    static ShaderEntry GetCubeShaderEntry();
};

VULKAN_NS_END