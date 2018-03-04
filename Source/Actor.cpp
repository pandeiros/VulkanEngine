/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
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
    vertexData = static_cast<Vertex*>(data);

    UpdateData();
}

void SceneComponent::UpdateData()
{
    if (!vertexData)
    {
        return;
    }

    uint32_t size, stride;
    void* data = renderComponent->GetData(size, stride);
    Vertex* originVertexData = static_cast<Vertex*>(data);

    uint32_t vertexCount = size / sizeof(Vertex);
    Transform transform = GetTransform();

    for (uint32_t i = 0; i < vertexCount; ++i)
    {
        (vertexData + i)->x = (originVertexData + i)->x * transform.scale.x;
        (vertexData + i)->y = (originVertexData + i)->y * transform.scale.y;
        (vertexData + i)->z = (originVertexData + i)->z * transform.scale.z;

        if (!bRotationSet)
        {
            glm::quat quatRotation = glm::quat(transform.rotation);
            glm::vec3 vertex = glm::vec3((vertexData + i)->x, (vertexData + i)->y, (vertexData + i)->z);
            vertex = quatRotation * vertex;
            (vertexData + i)->x = vertex.x;
            (vertexData + i)->y = vertex.y;
            (vertexData + i)->z = vertex.z;
        }

        (vertexData + i)->x += transform.position.x;
        (vertexData + i)->y += transform.position.y;
        (vertexData + i)->z += transform.position.z;

        (vertexData + i)->r = color.x;
        (vertexData + i)->g = color.y;
        (vertexData + i)->b = color.z;
    }

    bRotationSet = true;
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

void Actor::SetScale(glm::vec3 scale)
{
    transform.scale = scale;
}

void Actor::SetPosition(glm::vec3 position)
{
    transform.position = position;
}

void Actor::SetRotation(glm::vec3 rotation)
{
    transform.rotation = rotation;
}