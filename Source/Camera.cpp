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

Camera::Camera(CameraMode cameraMode, float yFovRadians, float aspectRatio, float zNear, float zFar, CameraTransform transform,
    const glm::mat4 clipMatrix /*= DEFAULT_CLIP_MATRIX*/)
    : cameraMode(cameraMode), yFOV(yFovRadians), zNear(zNear), zFar(zFar), aspectRatio(aspectRatio), transform(transform), clipMatrix(clipMatrix)
{
    UpdateMatrices();
}

glm::mat4 Camera::GetCameraMatrix() const
{
    return cameraMatrix;
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

glm::mat4 Camera::GetViewportMatrix() const
{
    return viewportMatrix;
}

void Camera::SetViewportMatrix(const glm::mat4 viewportMatrix)
{
    this->viewportMatrix = viewportMatrix;

    RecalculateCameraMatrix();
}

void Camera::Move(glm::vec3 positionDelta)
{
    transform.eye += positionDelta;

    UpdateViewMatrix();
}

void Camera::OnUpdateTransform(InputCode inputCode, InputEvent event, glm::mat4 value)
{
    if (inputCode == InputCode::GVR_LEFT_EYE_MATRIX || inputCode == InputCode::GVR_RIGHT_EYE_MATRIX)
    {
        //VULKAN_LOGGER("Left eye matrix: %s", glm::to_string(value));

        viewMatrix = value;
        UpdateViewMatrix();
    }
}

void Camera::UpdateMatrices()
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void Camera::UpdateViewMatrix()
{
    if (cameraMode == CameraMode::DEFAULT)
    {
        viewMatrix = glm::lookAt(transform.eye, transform.direction, transform.up);
    }
    else if (cameraMode == CameraMode::VR)
    {
        viewMatrix = glm::translate(viewMatrix, transform.eye);
    }

    RecalculateCameraMatrix();
}

void Camera::UpdateProjectionMatrix()
{
    projectionMatrix = glm::perspective(yFOV, aspectRatio, zNear, zFar);

    RecalculateCameraMatrix();
}

void Camera::RecalculateCameraMatrix()
{
    cameraMatrix = clipMatrix * viewportMatrix * projectionMatrix * viewMatrix;
}
