/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

/**
 * @file Primitive.h
 */

VULKAN_NS_BEGIN

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

#define XYZ1(X, Y, Z) (X), (Y), (Z), 1.f
#define COLOR_FLAT(R, G, B) XYZ1(R, G, B)
//#define UV(_u_, _v_) (_u_), (_v_)

/**
 * @class Primitive
 */
class Primitive
{
public:
    /**
     * Default constructor.
     */
    Primitive() = default;

    /**
     * Default destructor.
     */
    ~Primitive() = default;

    void* GetData(uint32_t& dataSize, uint32_t& dataStride);

protected:
    std::vector<Vertex> vertices;
};

/**
 * @class Cube
 */
class Cube : public Primitive
{
public:
    /**
     * Default destructor.
     */
    ~Cube() = default;

    Cube(float sideLength = 1.f);
};

VULKAN_NS_END