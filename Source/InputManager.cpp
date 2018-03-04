/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#include "InputManager.h"

#include "Core.h"

VULKAN_NS_USING;

InputManager::InputManager()
{
    InitInputs();
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

    Vector2D controllerPos { controllerState.GetTouchPos().x, controllerState.GetTouchPos().y };
    controllerPos.x = Math::MapToRange(controllerPos.x, 0.f, 1.f, -1.f, 1.f);
    controllerPos.y = Math::MapToRange(controllerPos.y, 0.f, 1.f, -1.f, 1.f);

    vector2DData.UpdateMappings(InputCode::GVR_TOUCHPAD, controllerState.IsTouching() ? InputState::DOWN : InputState::UP, controllerPos);
}

void InputManager::UpdateGVRHeadPose(gvr::GvrApi* gvrApi)
{
    // Obtain the latest, predicted head pose.
    gvr::ClockTimePoint timePoint = gvrApi->GetTimePointNow();

    glm::mat4 headMatrix = Math::Transpose(gvrApi->GetHeadSpaceFromStartSpaceTransform(timePoint));

#ifdef VULKAN_FIX_ANDROID_TRANSPOSE
    headMatrix = glm::rotate(headMatrix, glm::radians(90.f), glm::vec3(0.f, 0.f, -1.f));
#endif

    matrixData.UpdateMappings(InputCode::GVR_HEAD_MATRIX, InputState::ACTIVE, headMatrix);
    matrixData.UpdateMappings(InputCode::GVR_LEFT_EYE_MATRIX, InputState::ACTIVE, Math::Transpose(gvrApi->GetEyeFromHeadMatrix(GVR_LEFT_EYE)) * headMatrix);
    matrixData.UpdateMappings(InputCode::GVR_RIGHT_EYE_MATRIX, InputState::ACTIVE, Math::Transpose(gvrApi->GetEyeFromHeadMatrix(GVR_RIGHT_EYE)) * headMatrix);
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
