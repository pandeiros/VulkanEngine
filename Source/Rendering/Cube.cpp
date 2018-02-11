/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Rendering/Cube.h"

VULKAN_NS_USING;

//void Cube::SetColor(glm::vec3 color)
//{
//    for (Vertex& vertex : vertices)
//    {
//        vertex.r = color.x;
//        vertex.g = color.y;
//        vertex.b = color.z;
//    }
//}

//// #REFACTOR
//Cube::Cube(float sideLength)// , Transform transform)
//{
//    //this->transform = transform;
//
//    vertices.insert(vertices.end(), {
//        // red face
//        { XYZ1(-1, -1, 1), COLOR_FLAT(1.f, 0.f, 0.f) },
//        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 0.f, 0.f) },
//        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 0.f, 0.f) },
//        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 0.f, 0.f) },
//        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 0.f, 0.f) },
//        { XYZ1(1, 1, 1), COLOR_FLAT(1.f, 0.f, 0.f) },
//        // green face
//        { XYZ1(-1, -1, -1), COLOR_FLAT(0.f, 1.f, 0.f) },
//        { XYZ1(1, -1, -1), COLOR_FLAT(0.f, 1.f, 0.f) },
//        { XYZ1(-1, 1, -1), COLOR_FLAT(0.f, 1.f, 0.f) },
//        { XYZ1(-1, 1, -1), COLOR_FLAT(0.f, 1.f, 0.f) },
//        { XYZ1(1, -1, -1), COLOR_FLAT(0.f, 1.f, 0.f) },
//        { XYZ1(1, 1, -1), COLOR_FLAT(0.f, 1.f, 0.f) },
//        // blue face
//        { XYZ1(-1, 1, 1), COLOR_FLAT(0.f, 0.f, 1.f) },
//        { XYZ1(-1, -1, 1), COLOR_FLAT(0.f, 0.f, 1.f) },
//        { XYZ1(-1, 1, -1), COLOR_FLAT(0.f, 0.f, 1.f) },
//        { XYZ1(-1, 1, -1), COLOR_FLAT(0.f, 0.f, 1.f) },
//        { XYZ1(-1, -1, 1), COLOR_FLAT(0.f, 0.f, 1.f) },
//        { XYZ1(-1, -1, -1), COLOR_FLAT(0.f, 0.f, 1.f) },
//        // yellow face
//        { XYZ1(1, 1, 1), COLOR_FLAT(1.f, 1.f, 0.f) },
//        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 0.f) },
//        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 0.f) },
//        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 0.f) },
//        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 0.f) },
//        { XYZ1(1, -1, -1), COLOR_FLAT(1.f, 1.f, 0.f) },
//        // magenta face
//        { XYZ1(1, 1, 1), COLOR_FLAT(1.f, 0.f, 1.f) },
//        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 0.f, 1.f) },
//        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 0.f, 1.f) },
//        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 0.f, 1.f) },
//        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 0.f, 1.f) },
//        { XYZ1(-1, 1, -1), COLOR_FLAT(1.f, 0.f, 1.f) },
//        // cyan face
//        { XYZ1(1, -1, 1), COLOR_FLAT(0.f, 1.f, 1.f) },
//        { XYZ1(1, -1, -1), COLOR_FLAT(0.f, 1.f, 1.f) },
//        { XYZ1(-1, -1, 1), COLOR_FLAT(0.f, 1.f, 1.f) },
//        { XYZ1(-1, -1, 1), COLOR_FLAT(0.f, 1.f, 1.f) },
//        { XYZ1(1, -1, -1), COLOR_FLAT(0.f, 1.f, 1.f) },
//        { XYZ1(-1, -1, -1), COLOR_FLAT(0.f, 1.f, 1.f) }
//    });
//}

VertexData Cube::GetCubeVertexData()
{
    return {
        // red face
        { XYZ1(-1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        // green face
        { XYZ1(-1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        // blue face
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        // yellow face
        { XYZ1(1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        // magenta face
        { XYZ1(1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        // cyan face
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) }
    };
}

ShaderEntry Cube::GetCubeShaderEntry()
{
    ShaderEntry shaderEntry;

    shaderEntry.vertexShaders.push_back(VULKAN_VERTEX_SHADER_TEXT);
    shaderEntry.fragmentShaders.push_back(VULKAN_FRAGMENT_SHADER_TEXT);

    return shaderEntry;
}
