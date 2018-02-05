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

    floatData.UpdateMappings(InputCode::GVR_BUTTON_APP, MapToButtonInputState(controllerState.GetButtonState(gvr::kControllerButtonApp)), 0.f);
    floatData.UpdateMappings(InputCode::GVR_BUTTON_CLICK, MapToButtonInputState(controllerState.GetButtonState(gvr::kControllerButtonClick)), 0.f);

    vector2DData.UpdateMappings(InputCode::GVR_TOUCHPAD, controllerState.GetTouchDown() ? InputState::DOWN : InputState::UP, { controllerState.GetTouchPos().x, controllerState.GetTouchPos().y });
}

void InputManager::UpdateGVRHeadPose(gvr::GvrApi* gvrApi)
{
    // Obtain the latest, predicted head pose.
    gvr::ClockTimePoint timePoint = gvrApi->GetTimePointNow();

    glm::mat4 headMatrix = Math::Transpose(gvrApi->GetHeadSpaceFromStartSpaceRotation(timePoint));

    matrixData.UpdateMappings(InputCode::GVR_HEAD_MATRIX, InputState::ACTIVE, headMatrix);
    matrixData.UpdateMappings(InputCode::GVR_LEFT_EYE_MATRIX, InputState::ACTIVE, Math::Transpose(gvrApi->GetEyeFromHeadMatrix(GVR_LEFT_EYE)) * headMatrix);
    matrixData.UpdateMappings(InputCode::GVR_RIGHT_EYE_MATRIX, InputState::ACTIVE, Math::Transpose(gvrApi->GetEyeFromHeadMatrix(GVR_RIGHT_EYE)) * headMatrix);



    //headMatrix = Math::Transpose(gvrApi->GetHeadSpaceFromStartSpaceRotation(timePoint));
    //leftEyeMatrix = 10.f * Math::Transpose(gvrApi->GetEyeFromHeadMatrix(GVR_LEFT_EYE)) * headMatrix;
    //rightEyeMatrix = Math::Transpose(gvrApi->GetEyeFromHeadMatrix(GVR_RIGHT_EYE)) * headMatrix;
}

void InputManager::InitGVRInput()
{
    floatData.GetData().insert(std::make_pair(InputCode::GVR_BUTTON_APP, InputData<float>(InputState::UP, InputType::BUTTON, 0.f)));
    floatData.GetData().insert(std::make_pair(InputCode::GVR_BUTTON_CLICK, InputData<float>(InputState::UP, InputType::BUTTON, 0.f)));

    vector2DData.GetData().insert(std::make_pair(InputCode::GVR_TOUCHPAD, InputData<Vector2D>(InputState::UP, InputType::TOUCH, {})));

    matrixData.GetData().insert(std::make_pair(InputCode::GVR_HEAD_MATRIX, InputData<glm::mat4>(InputState::INACTIVE, InputType::CONTINUOUS, glm::mat4(1.f))));
    matrixData.GetData().insert(std::make_pair(InputCode::GVR_LEFT_EYE_MATRIX, InputData<glm::mat4>(InputState::INACTIVE, InputType::CONTINUOUS, glm::mat4(1.f))));
    matrixData.GetData().insert(std::make_pair(InputCode::GVR_RIGHT_EYE_MATRIX, InputData<glm::mat4>(InputState::INACTIVE, InputType::CONTINUOUS, glm::mat4(1.f))));
}

#endif
