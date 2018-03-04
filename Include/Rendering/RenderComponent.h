/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Utils/Math.h"

/**
 * @file RenderComponent.h
 */

VULKAN_NS_BEGIN

#define XYZ1(X, Y, Z) (X), (Y), (Z), 1.f
#define COLOR_FLAT(R, G, B) XYZ1(R, G, B)

struct Vertex
{
    // Position
    float x, y, z, w;

    // Color
    float r, g, b, a;
};

struct VertexUV
{
    // Position
    float x, y, z, w;

    // UV
    float u, v;
};

typedef std::vector<Vertex> VertexData;
typedef std::vector<size_t> ShaderIndexData;
static const Transform DEFAULT_TRANSFORM = Transform(glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f));

/**
 * @class RenderComponent
 */
class RenderComponent
{
public:
    void* GetData(uint32_t& dataSize, uint32_t& dataStride);

    void SetVertexData(VertexData& vertexData);
    ShaderIndexData& GetShaderIndexData();

protected:
    VertexData vertices;
    ShaderIndexData shaders;
};

VULKAN_NS_END