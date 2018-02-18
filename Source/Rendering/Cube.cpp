/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Rendering/Cube.h"

VULKAN_NS_USING;

VertexData Cube::GetCubeVertexData()
{
    return {
        { XYZ1(-1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },

        { XYZ1(-1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },

        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },

        { XYZ1(1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, -1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },

        { XYZ1(1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, 1), COLOR_FLAT(1.f, 1.f, 1.f) },
        { XYZ1(-1, 1, -1), COLOR_FLAT(1.f, 1.f, 1.f) },

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
