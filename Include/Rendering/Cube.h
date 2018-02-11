/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Shape.h"
#include "Shaders.h"

/**
 * @file Cube.h
 */

VULKAN_NS_BEGIN

/**
 * @class Cube
 */
class Cube : public Shape
{
public:
    /**
     * Default destructor.
     */
    ~Cube() = default;

    //void SetColor(glm::vec3 color);
    //Cube(float sideLength = 1.f); // , Transform transform = DEFAULT_TRANSFORM);

    static VertexData GetCubeVertexData();
    static ShaderEntry GetCubeShaderEntry();
};

VULKAN_NS_END