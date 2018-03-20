/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Object.h"
#include "Rendering/RenderComponent.h"
#include "InputManager.h"

/**
 * @file Actor.h
 */

VULKAN_NS_BEGIN

class Actor;

/**
 * @class SceneComponent
 */
class SceneComponent : public Object
{
public:
    void SetOwner(Actor* actor);

    void SetRenderComponent(RenderComponent* renderComponent);
    virtual void SetColor(const glm::vec3 color);

    RenderComponent* GetRenderComponent() const;

    Transform GetTransform() const;

    void ApplyTransformAndColor(void* data);
    virtual void UpdateData();

    Vertex* vertexData = nullptr;

protected:
    RenderComponent* renderComponent;

    Transform transform = Transform();
    glm::vec3 color;

    Actor* owner = nullptr;

    bool bRotationSet = false;
};

/**
 * @class Actor
 */
class Actor : public Object
{
public:
    Actor() = default;

    ~Actor() = default;

    void SetSceneComponent(SceneComponent* sceneComponent);
    SceneComponent* GetSceneComponent() const;

    void SetTransform(Transform newTransform);
    Transform GetTransform() const;

    void SetScale(glm::vec3 scale);
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);

protected:
    std::unique_ptr<SceneComponent> sceneComponent;

    Transform transform = Transform();
};

VULKAN_NS_END