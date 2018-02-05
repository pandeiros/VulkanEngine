/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "Camera.h"

#include "Core.h"

VULKAN_NS_USING;

const glm::mat4 Camera::DEFAULT_CLIP_MATRIX = glm::mat4(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, -1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.5f, 0.0f,
    0.0f, 0.0f, 0.5f, 1.0f);

Camera::Camera(float yFovRadians, float aspectRatio, float zNear, float zFar, CameraTransform transform,
    const glm::mat4 clipMatrix /*= DEFAULT_CLIP_MATRIX*/)
    : yFOV(yFovRadians), zNear(zNear), zFar(zFar), aspectRatio(aspectRatio), transform(transform), clipMatrix(clipMatrix)
{
    UpdateMatrices();
}

glm::mat4 Camera::GetViewProjectionMatrix() const
{
    return viewProjectionClipMatrix;
}

glm::mat4 Camera::GetViewMatrix() const
{
    return viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return projectionMatrix;
}

glm::mat4 Camera::GetClipMatrix() const
{
    return clipMatrix;
}

void Camera::Move(glm::vec3 positionDelta)
{
    transform.eye += positionDelta;
    UpdateMatrices();
}

void Camera::OnUpdateTransform(InputCode inputCode, InputEvent event, glm::mat4 value)
{
    if (inputCode == InputCode::GVR_LEFT_EYE_MATRIX)
    {
        VULKAN_LOGGER("Left eye matrix: %s", glm::to_string(value));
    }
}

void Camera::UpdateMatrices()
{
    projectionMatrix = glm::perspective(yFOV, aspectRatio, zNear, zFar);
    viewMatrix = glm::lookAt(transform.eye, transform.direction, transform.up);

    viewProjectionClipMatrix = clipMatrix * projectionMatrix * viewMatrix;
}
