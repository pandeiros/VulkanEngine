/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Utils/Math.h"

/**
 * @file Camera.h
 */

VULKAN_NS_BEGIN

/**
 * @class Actor
 */
struct CameraTransform
{
    glm::vec3 eye;
    glm::vec3 direction;
    glm::vec3 up;
};

/**
 * @class Camera
 */
class Camera
{
public:
    static const glm::mat4 DEFAULT_CLIP_MATRIX;

    /**
     * Default constructor.
     */
    Camera() = default;

    /**
     * Default destructor.
     */
    ~Camera() = default;

    /** yFOV in degrees. */
    Camera(float yFovRadians, float aspectRatio, float zNear, float zFar, CameraTransform transform,
        const glm::mat4 clipMatrix = DEFAULT_CLIP_MATRIX);

    glm::mat4 GetViewProjectionMatrix() const;

    void Move(glm::vec3 positionDelta);

private:
    void UpdateMatrices();

    // In radians.
    float yFOV;
    float aspectRatio;
    float zNear;
    float zFar;
    CameraTransform transform;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 clipMatrix;
    glm::mat4 viewProjectionClipMatrix;
};

VULKAN_NS_END