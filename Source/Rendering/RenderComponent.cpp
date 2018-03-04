/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#include "Rendering/RenderComponent.h"

VULKAN_NS_USING;

void* RenderComponent::GetData(uint32_t& dataSize, uint32_t& dataStride)
{
    dataSize = (uint32_t)vertices.size() * sizeof(Vertex);
    dataStride = sizeof(Vertex);

    return vertices.data();
}

void RenderComponent::SetVertexData(VertexData& vertexData)
{
    vertices = vertexData;
}

ShaderIndexData& RenderComponent::GetShaderIndexData()
{
    return shaders;
}
