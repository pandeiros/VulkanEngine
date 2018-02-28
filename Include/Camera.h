/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Utils/Math.h"
#include "InputManager.h"

/**
 * @file Camera.h
 */

VULKAN_NS_BEGIN

enum CameraMode
{
    DEFAULT,
    VR
};

/**
 * @struct CameraTransform
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

    /** */
    Camera(CameraMode cameraMode, float yFovRadians, float aspectRatio, float zNear, float zFar, CameraTransform transform,
        const glm::mat4 clipMatrix = DEFAULT_CLIP_MATRIX);

    CameraMode GetMode() const;

    glm::mat4 GetCameraMatrix() const;

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetClipMatrix() const;
    glm::mat4 GetViewportMatrix() const;
    glm::mat4 GetCorrectedProjectionMatrix() const;
    glm::vec4 GetLensUndistortionCoefficients() const;

    void SetViewportMatrix(const glm::mat4 viewportMatrix);
    void SetLensUndistortionCoefficients(glm::vec4 coefficients);

    void Move(glm::vec3 positionDelta);

    void OnUpdateTransform(InputCode inputCode, InputEvent event, glm::mat4 value);
    void OnUpdatePosition(InputCode inputCode, InputEvent event, Vector2D value);
    void OnModeChange(InputCode inputCode, InputEvent event, float value);

private:
    void UpdateMatrices();
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
    void RecalculateCameraMatrix();

    // In radians.
    float yFOV;
    float aspectRatio;
    float zNear;
    float zFar;
    CameraTransform transform;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 clipMatrix;
    glm::mat4 viewportMatrix;

    glm::mat4 cameraMatrix;

    glm::vec4 undistortionCoefficients;
    bool bUndistortionEnabled = false;

    CameraMode cameraMode;

    bool bRotationEnabled = false;;
};

VULKAN_NS_END