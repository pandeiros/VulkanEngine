/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "InputManager.h"

#include "Core.h"

VULKAN_NS_USING;

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

InputState InputManager::MapToButtonInputState(bool value)
{
    return (value ? InputState::DOWN : InputState::UP);
}

#ifdef __ANDROID__

void InputManager::UpdateGVRControllerState(gvr::ControllerApi* controllerApi)
{
    controllerState.Update(*controllerApi);

    buttonData.UpdateMappings(InputCodes::GVR_BUTTON_APP, MapToButtonInputState(controllerState.GetButtonState(gvr::kControllerButtonApp)), 0.f);
    buttonData.UpdateMappings(InputCodes::GVR_BUTTON_CLICK, MapToButtonInputState(controllerState.GetButtonState(gvr::kControllerButtonClick)), 0.f);

    vector2DData.UpdateMappings(InputCodes::GVR_TOUCHPAD, controllerState.GetTouchDown() ? InputState::DOWN : InputState::UP, { controllerState.GetTouchPos().x, controllerState.GetTouchPos().y });
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
    buttonData.GetData().insert(std::make_pair(InputCodes::GVR_BUTTON_APP, InputData<float>(InputState::UP, InputType::BUTTON, 0.f)));
    buttonData.GetData().insert(std::make_pair(InputCodes::GVR_BUTTON_CLICK, InputData<float>(InputState::UP, InputType::BUTTON, 0.f)));

    vector2DData.GetData().insert(std::make_pair(InputCodes::GVR_TOUCHPAD, InputData<Vector2D>(InputState::UP, InputType::TOUCH, {})));
}

#endif
