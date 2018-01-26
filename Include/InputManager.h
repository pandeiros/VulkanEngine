/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Utils/Math.h"

#ifdef __ANDROID__
#include "gvr.h"
#include "gvr_controller.h"
#endif

#include <map>
#include <chrono>

/**
 * @file InputManager.h
 */

VULKAN_NS_BEGIN

/**
 *
 */
enum InputState
{
    DOWN,
    UP,
    PRESSED,
    ACTIVE,
    INACTIVE
};

enum InputType
{
    BUTTON,
    TOUCH,
    CONTINUOUS
};

enum InputEvent
{
    ON_DOWN,
    ON_UP,
    ON_PRESSED,
    ON_LONG_PRESSED,
    ON_CHANGE
};

enum InputCodes
{
    ANY = 0x0,

    // Android
    GVR_BUTTON_APP = 0x100,
    GVR_BUTTON_CLICK = 0x101,
    GVR_TOUCHPAD = 0x102
};

template<typename T>
class InputStorage
{
    InputStorage()
    {}

    InputStorage(T value, InputState state, InputType type)
        : value(value), state(state), type(type)
    {}

    T value;
    InputState state;
    InputType type;
    std::chrono::steady_clock::time_point startTime;
};

/**
 * @class InputManager
 */
class InputManager
{
public:
    InputManager();

    // #TODO Change this so that camera registers a delegate that pass view matrices to it.
    glm::mat4 GetHeadMatrix();
    glm::mat4 GetLeftEyeMatrix();
    glm::mat4 GetRightEyeMatrix();

private:
    void InitInputs();
    void UpdateButtonMapping(InputCodes inputCode, InputState newState);

    std::map<InputCodes, InputState> buttonMappings;
    std::map<InputCodes, InputStorage<Vector2D>> vector2DMappings;

    glm::mat4 headMatrix;
    glm::mat4 leftEyeMatrix;
    glm::mat4 rightEyeMatrix;

    std::chrono::steady_clock timer;

#ifdef __ANDROID__
public:
    void UpdateGVRControllerState(gvr::ControllerApi* controllerApi);
    void UpdateGVRHeadPose(gvr::GvrApi* gvrApi);

private:
    void InitGVRInput();
    InputState MapToInputState(bool value);

    // GVR Controller
    gvr::ControllerState controllerState;
#endif
};

VULKAN_NS_END