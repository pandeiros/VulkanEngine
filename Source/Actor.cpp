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
