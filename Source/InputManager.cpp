/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "InputManager.h"

#include "Core.h"

VULKAN_NS_USING;

VK_DECLARE_LOG_CATEGORY(LogInputManager);

InputManager::InputManager()
{
    InitInputs();

    headMatrix = leftEyeMatrix = rightEyeMatrix = glm::mat4(1.f);
}

glm::mat4 InputManager::GetHeadMatrix()
{
    return headMatrix;
}

glm::mat4 InputManager::GetLeftEyeMatrix()
{
    return leftEyeMatrix;
}

glm::mat4 InputManager::GetRightEyeMatrix()
{
    return rightEyeMatrix;
}

void InputManager::InitInputs()
{
#ifdef __ANDROID__
    InitGVRInput();
#endif
}

void InputManager::UpdateButtonMapping(InputCodes inputCode, InputState newState)
{
    InputState& previousState = buttonMappings.at(inputCode);

    if (previousState == newState)
    {
        // DO NOTHING
    }
    else if (previousState == InputState::DOWN && newState == InputState::UP)
    {
        // Event ON_UP
        VK_LOG(LogInputManager, Debug, "Input code %u %u", (uint32_t)inputCode, (uint32_t)newState);
    }
    else if (previousState == InputState::UP && newState == InputState::DOWN)
    {
        // Event ON_DOWN
    }
    else
    {
        // Wrong states!
    }

    previousState = newState;
}

#ifdef __ANDROID__

void InputManager::UpdateGVRControllerState(gvr::ControllerApi* controllerApi)
{
    controllerState.Update(*controllerApi);

    UpdateButtonMapping(InputCodes::GVR_BUTTON_APP, MapToInputState(controllerState.GetButtonState(gvr::kControllerButtonApp)));
    UpdateButtonMapping(InputCodes::GVR_BUTTON_CLICK, MapToInputState(controllerState.GetButtonState(gvr::kControllerButtonClick)));

    //vector2DMappings
}

void InputManager::UpdateGVRHeadPose(gvr::GvrApi* gvrApi)
{
    // Obtain the latest, predicted head pose.
    gvr::ClockTimePoint timePoint = gvrApi->GetTimePointNow();

    headMatrix = Math::Transpose(gvrApi->GetHeadSpaceFromStartSpaceRotation(timePoint));
    leftEyeMatrix = 10.f * Math::Transpose(gvrApi->GetEyeFromHeadMatrix(GVR_LEFT_EYE)) * headMatrix;
    rightEyeMatrix = Math::Transpose(gvrApi->GetEyeFromHeadMatrix(GVR_RIGHT_EYE)) * headMatrix;
}

void InputManager::InitGVRInput()
{
    buttonMappings.insert(std::make_pair(InputCodes::GVR_BUTTON_APP, InputState::UP));
    buttonMappings.insert(std::make_pair(InputCodes::GVR_BUTTON_CLICK, InputState::UP));
    //vector2DMappings.insert_or_assign(InputCodes::GVR_TOUCHPAD, InputStorage<Vector2D>({0.f, 0.f}, InputState::UP, InputType::TOUCH));
}

InputState InputManager::MapToInputState(bool value)
{
    return (value ? InputState::DOWN : InputState::UP);
}

#endif
