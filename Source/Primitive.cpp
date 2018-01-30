/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Primitive.h"

VULKAN_NS_USING;

void Primitive::UpdateVertices()
{
    for (Vertex& vertex : vertices)
    {
        vertex.x *= transform.scale.x;
        vertex.y *= transform.scale.y;
        vertex.z *= transform.scale.z;

        vertex.x += transform.position.x;
        vertex.y += transform.position.y;
        vertex.z += transform.position.z;
    }
}

void* Primitive::GetData(uint32_t& dataSize, uint32_t& dataStride)
{
    dataSize = (uint32_t)vertices.size() * sizeof(Vertex);
    dataStride = sizeof(Vertex);

    return vertices.data();
}

void Cube::SetColor(glm::vec3 color)
{
    for (Vertex& vertex : vertices)
    {
        vertex.r = color.x;
        vertex.g = color.y;
        vertex.b = color.z;
    }
}

// #REFACTOR
Cube::Cube(float sideLength, Transform transform)
    : Primitive(transform)
{
    vertices.insert(vertices.end(), {
        // red face
        { XYZ1(-1, -1, 1), COLOR_FLAT(1.f, 0.f, 0.f) },
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 0.f, 0.f) },
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 0.f, 0.f) },
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 0.f, 0.f) },
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 0.f, 0.f) },
        { XYZ1(1, 1, 1), COLOR_FLAT(1.f, 0.f, 0.f) },
        // green face
        { XYZ1(-1, -1, -1), COLOR_FLAT(0.f, 1.f, 0.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(0.f, 1.f, 0.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(0.f, 1.f, 0.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(0.f, 1.f, 0.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(0.f, 1.f, 0.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(0.f, 1.f, 0.f) },
        // blue face
        { XYZ1(-1, 1, 1), COLOR_FLAT(0.f, 0.f, 1.f) },
        { XYZ1(-1, -1, 1), COLOR_FLAT(0.f, 0.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(0.f, 0.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(0.f, 0.f, 1.f) },
        { XYZ1(-1, -1, 1), COLOR_FLAT(0.f, 0.f, 1.f) },
        { XYZ1(-1, -1, -1), COLOR_FLAT(0.f, 0.f, 1.f) },
        // yellow face
        { XYZ1(1, 1, 1), COLOR_FLAT(1.f, 1.f, 0.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 0.f) },
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 0.f) },
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 0.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 0.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(1.f, 1.f, 0.f) },
        // magenta face
        { XYZ1(1, 1, 1), COLOR_FLAT(1.f, 0.f, 1.f) },
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 0.f, 1.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 0.f, 1.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 0.f, 1.f) },
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 0.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(1.f, 0.f, 1.f) },
        // cyan face
        { XYZ1(1, -1, 1), COLOR_FLAT(0.f, 1.f, 1.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(0.f, 1.f, 1.f) },
        { XYZ1(-1, -1, 1), COLOR_FLAT(0.f, 1.f, 1.f) },
        { XYZ1(-1, -1, 1), COLOR_FLAT(0.f, 1.f, 1.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(0.f, 1.f, 1.f) },
        { XYZ1(-1, -1, -1), COLOR_FLAT(0.f, 1.f, 1.f) }
    });
}
