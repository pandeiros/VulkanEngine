/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Object.h"
#include "Rendering/RenderComponent.h"

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
    void SetColor(const glm::vec3 color);

    RenderComponent* GetRenderComponent() const;

    Transform GetTransform() const;

    void ApplyTransformAndColor(void* data);

private:
    RenderComponent* renderComponent;

    Transform transform;
    glm::vec3 color;

    Actor* owner = nullptr;
};

/**
 * @class Actor
 */
class Actor : public Object
{
public:
    /**
     * Default constructor.
     */
    Actor() = default;

    /**
     * Default destructor.
     */
    ~Actor() = default;

    void SetSceneComponent(SceneComponent* sceneComponent);
    SceneComponent* GetSceneComponent() const;

    void SetTransform(Transform newTransform);
    Transform GetTransform() const;

    //void* GetData(uint32_t& dataSize, uint32_t& dataStride);

protected:
    std::unique_ptr<SceneComponent> sceneComponent;

    Transform transform;
};

VULKAN_NS_END