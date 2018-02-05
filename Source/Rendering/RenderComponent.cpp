/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Rendering/RenderComponent.h"

VULKAN_NS_USING;

void RenderComponent::UpdateVertices()
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

void* RenderComponent::GetData(uint32_t& dataSize, uint32_t& dataStride)
{
    dataSize = (uint32_t)vertices.size() * sizeof(Vertex);
    dataStride = sizeof(Vertex);

    return vertices.data();
}