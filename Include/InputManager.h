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
#include <functional>

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
    ON_DOWN             = 1,
    ON_UP               = 1 << 1,
    ON_PRESSED          = 1 << 2,
    ON_LONG_PRESSED     = 1 << 3,
    ON_CHANGE           = 1 << 4,
    ON_ACTIVE              = 1 << 5
};

enum InputCode
{
    ANY = 0x0,

    // Android
    GVR_BUTTON_APP = 0x100,
    GVR_BUTTON_CLICK = 0x101,
    GVR_TOUCHPAD = 0x102,
    GVR_HEAD_MATRIX = 0x103,
    GVR_LEFT_EYE_MATRIX = 0x104,
    GVR_RIGHT_EYE_MATRIX = 0x105
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

#define INPUT_EVENT_BINDING(T) std::function<void(InputCode, InputEvent, T)>

template<typename T>
struct InputEventDelegate
{
    InputEventDelegate()
    {}

    InputEventDelegate(uint32_t inputEventFlags, INPUT_EVENT_BINDING(T) binding)
        : inputEventFlags(inputEventFlags), binding(binding)
    {}

    INPUT_EVENT_BINDING(T) binding;
    uint32_t inputEventFlags = 0;
};

template<typename T>
class InputStorage
{
public:
    InputStorage()
    {}

    std::map<InputCode, InputData<T>>& GetData();

    void UpdateMappings(InputCode inputCode, InputState newState, T newValue);
    void BroadcastEvents(InputCode inputCode, InputEvent event, T value);
    void AddDelegate(InputCode inputCode, InputEventDelegate<T> eventDelegate);

private:
    std::map<InputCode, InputData<T>> data;

    std::map<InputCode, std::vector<InputEventDelegate<T>>> eventDelegates;
};

template<typename T>
std::map<InputCode, InputData<T>>& InputStorage<T>::GetData()
{
    return data;
}

template<typename T>
void InputStorage<T>::UpdateMappings(InputCode inputCode, InputState newState, T newValue)
{
    auto& input = data.at(inputCode);

    if (input.state == newState)
    {

    }
    else if (input.state == InputState::DOWN && newState == InputState::UP)
    {
        //VK_LOG(LogInputManager, Debug, "Input code %u: %u", (uint32_t)inputCode, (uint32_t)newState);
        BroadcastEvents(inputCode, newState, newValue);
    }
    else if (input.state == InputState::UP && newState == InputState::DOWN)
    {
        //VK_LOG(LogInputManager, Debug, "Input code %u: %u", (uint32_t)inputCode, (uint32_t)newState);
        BroadcastEvents(inputCode, newState, newValue);
    }
    else
    {
        // Wrong states!
    }

    input.state = newState;
};

template<typename T>
void InputStorage<T>::BroadcastEvents(InputCode inputCode, InputEvent event, T value)
{
    if (eventDelegates.find(inputCode) != eventDelegates.end())
    {
        for (InputEventDelegate<T>& eventDelegate : eventDelegates.at(inputCode))
        {
            if (eventDelegate.binding && eventDelegate.inputEventFlags & event)
            {
                eventDelegate.binding(inputCode, event, value);
            }
        }
    }
}

template<typename T>
void InputStorage<T>::AddDelegate(InputCode inputCode, InputEventDelegate<T> eventDelegate)
{
    if (eventDelegates.find(inputCode) == eventDelegates.end())
    {
        eventDelegates.insert(std::make_pair(inputCode, std::vector<InputEventDelegate<T>>()));
    }

    eventDelegates.at(inputCode).push_back(eventDelegate);
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

    template<typename T>
    void BindEvent(InputCode inputCode, InputEventDelegate<T> eventDelegate)
    {
        VK_ASSERT(0, "BindEvent: unsupported type %s", typeid(value).name());
    }

private:
    void InitInputs();

    InputStorage<float> floatData;
    InputStorage<Vector2D> vector2DData;
    InputStorage<glm::mat4> matrixData;

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

template<>
inline void InputManager::BindEvent<float>(InputCode inputCode, InputEventDelegate<float> eventDelegate)
{
    floatData.AddDelegate(inputCode, eventDelegate);
}

template<>
inline void InputManager::BindEvent<Vector2D>(InputCode inputCode, InputEventDelegate<Vector2D> eventDelegate)
{
    vector2DData.AddDelegate(inputCode, eventDelegate);
}

template<>
inline void InputManager::BindEvent<glm::mat4>(InputCode inputCode, InputEventDelegate<glm::mat4> eventDelegate)
{
    matrixData.AddDelegate(inputCode, eventDelegate);
}


VULKAN_NS_END