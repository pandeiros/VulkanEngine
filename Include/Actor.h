/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
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
    void SetColor(const glm::vec3 color);

    RenderComponent* GetRenderComponent() const;

    Transform GetTransform() const;

    void ApplyTransformAndColor(void* data);
    void UpdateData();

    Vertex* vertexData = nullptr;

private:
    RenderComponent* renderComponent;

    Transform transform = Transform();
    glm::vec3 color;

    Actor* owner = nullptr;

    bool bRotationSet = false; // #TODO Temp
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

    void SetScale(glm::vec3 scale);
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);

    //void* GetData(uint32_t& dataSize, uint32_t& dataStride);

protected:
    std::unique_ptr<SceneComponent> sceneComponent;

    Transform transform = Transform();
};

/**
* @class Actor
*/
class TestActor : public Actor
{
public:
    virtual void Tick(float deltaTime);

    void OnModeChange(InputCode inputCode, InputEvent event, float value);

    glm::vec3 originPos;
    glm::vec3 destPos;

    bool bAnimSwitch = false;

    float timer = 0.f;
};

VULKAN_NS_END