/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Utils/Math.h"

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

struct Transform
{
    Transform()
    {
        position = glm::vec3(0.f, 0.f, 0.f);
        scale = glm::vec3(1.f, 1.f, 1.f);
    }

    Transform(glm::vec3 position, glm::vec3 scale)
        : position(position), scale(scale)
    {}

    glm::vec3 position;
    glm::vec3 scale;
};

static const Transform DEFAULT_TRANSFORM = Transform(glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f));

#define XYZ1(X, Y, Z) (X), (Y), (Z), 1.f
#define COLOR_FLAT(R, G, B) XYZ1(R, G, B)
//#define UV(_u_, _v_) (_u_), (_v_)

/**
 * @class Primitive
 */
class Primitive
{
public:
    Primitive(Transform transform)
        : transform(transform)
    {}

    void UpdateVertices();

    void* GetData(uint32_t& dataSize, uint32_t& dataStride);

protected:
    std::vector<Vertex> vertices;

    Transform transform;
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

    void SetColor(glm::vec3 color);
    Cube(float sideLength = 1.f, Transform transform = DEFAULT_TRANSFORM);
};

VULKAN_NS_END