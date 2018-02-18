/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Actor.h"

VULKAN_NS_USING;

void SceneComponent::SetOwner(Actor* actor)
{
    owner = actor;
}

void SceneComponent::SetRenderComponent(RenderComponent* renderComponent)
{
    this->renderComponent = renderComponent;
}

void SceneComponent::SetColor(const glm::vec3 color)
{
    this->color = color;
}

RenderComponent* SceneComponent::GetRenderComponent() const
{
    return renderComponent;
}

Transform SceneComponent::GetTransform() const
{
    return owner ? owner->GetTransform() + transform : transform;
}

void SceneComponent::ApplyTransformAndColor(void* data)
{
    uint32_t size, stride;
    renderComponent->GetData(size, stride);

    Vertex* vertexData = static_cast<Vertex*>(data);
    uint32_t vertexCount = size / sizeof(Vertex);
    Transform transform = GetTransform();

    for (uint32_t i = 0; i < vertexCount; ++i)
    {
        (vertexData + i)->x *= transform.scale.x;
        (vertexData + i)->y *= transform.scale.y;
        (vertexData + i)->z *= transform.scale.z;

        (vertexData + i)->x += transform.position.x;
        (vertexData + i)->y += transform.position.y;
        (vertexData + i)->z += transform.position.z;

        (vertexData + i)->r = color.x;
        (vertexData + i)->g = color.y;
        (vertexData + i)->b = color.z;
    }
}

void Actor::SetSceneComponent(SceneComponent* sceneComponent)
{
    this->sceneComponent.reset(sceneComponent);
    sceneComponent->SetOwner(this);
}

SceneComponent* Actor::GetSceneComponent() const
{
    return sceneComponent.get();
}

void Actor::SetTransform(const Transform newTransform)
{
    transform = newTransform;
}

Transform Actor::GetTransform() const
{
    return transform;
}
