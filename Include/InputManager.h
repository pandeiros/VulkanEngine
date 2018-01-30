/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#include "Core.h"

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

VK_DECLARE_LOG_CATEGORY(LogInputManager);

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
    ON_CHANGE,
    ALWAYS
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
struct InputData
{
    InputData()
    {}

    InputData(InputState state, InputType type, T value)
        : value(value), state(state), type(type)
    {}

    T value;
    InputState state;
    InputType type;
    std::chrono::steady_clock::time_point startTime;
};

template<typename T>
class InputStorage
{
public:
    InputStorage()
    {}

    void UpdateMappings(InputCodes inputCode, InputState newState, T newValue);
    void BroadcastEvents(InputCodes inputCode, InputEvent event, T value);

    std::map<InputCodes, InputData<T>>& GetData();

private:
    std::map<InputCodes, InputData<T>> data;
};

template<typename T>
std::map<InputCodes, InputData<T>>& InputStorage<T>::GetData()
{
    return data;
}

template<typename T>
void InputStorage<T>::UpdateMappings(InputCodes inputCode, InputState newState, T newValue)
{
    auto& input = data.at(inputCode);

    if (input.state == newState)
    {

    }
    else if (input.state == InputState::DOWN && newState == InputState::UP)
    {
        // Event ON_UP
        VK_LOG(LogInputManager, Debug, "Input code %u: %u", (uint32_t)inputCode, (uint32_t)newState);
    }
    else if (input.state == InputState::UP && newState == InputState::DOWN)
    {
        // Event ON_DOWN
        VK_LOG(LogInputManager, Debug, "Input code %u: %u", (uint32_t)inputCode, (uint32_t)newState);
    }
    else
    {
        // Wrong states!
    }

    input.state = newState;
};

template<typename T>
void InputStorage<T>::BroadcastEvents(InputCodes inputCode, InputEvent event, T value)
{

}

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

    InputStorage<float> buttonData;
    InputStorage<Vector2D> vector2DData;

    glm::mat4 headMatrix;
    glm::mat4 leftEyeMatrix;
    glm::mat4 rightEyeMatrix;

    std::chrono::steady_clock timer;

    InputState MapToButtonInputState(bool value);

    //////////////////////////////////////////////////////////////////////////
    // Events
    //////////////////////////////////////////////////////////////////////////

#ifdef __ANDROID__
public:
    void UpdateGVRControllerState(gvr::ControllerApi* controllerApi);
    void UpdateGVRHeadPose(gvr::GvrApi* gvrApi);

private:
    void InitGVRInput();

    // GVR Controller
    gvr::ControllerState controllerState;
#endif
};

VULKAN_NS_END