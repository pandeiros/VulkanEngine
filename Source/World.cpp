/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "World.h"

#include "Core.h"
#include "Engine.h"

VULKAN_NS_USING;

VK_DECLARE_LOG_CATEGORY(LogWorld);

World::World(DevicePtr device, Engine* engine)
    : VulkanClass (device), engine(engine)
{

}

World::~World()
{
    cameras.clear();
}

void World::Tick(float deltaTime)
{
    VK_PERFORMANCE_SECTION("World update");
}

void World::SetCamera(CameraMode cameraMode, float yFovDegrees, float aspectRatio, float zNear, float zFar)
{
    using namespace std::placeholders;

    float fov = glm::radians(yFovDegrees);
    if (aspectRatio > 1.f)
    {
        fov *= 1.f / aspectRatio;
    }

    if (cameraMode == CameraMode::DEFAULT)
    {
        Camera* camera = new Camera(fov, aspectRatio, 0.1f, 100.f,
        { glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0) }, Camera::DEFAULT_CLIP_MATRIX);

        cameras.push_back(std::unique_ptr<Camera>(std::move(camera)));
    }
    else if (cameraMode == CameraMode::VR)
    {
        Camera* leftEyeCamera = new Camera(fov, aspectRatio, 0.1f, 100.f,
        { glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0) });
        Camera* rightEyeCamera = new Camera(fov, aspectRatio, 0.1f, 100.f,
        { glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0) });

        engine->GetInputManager()->BindEvent<glm::mat4>(InputCode::GVR_LEFT_EYE_MATRIX,
            InputEventDelegate<glm::mat4>(InputEvent::ON_ACTIVE, std::bind(&Camera::OnUpdateTransform, leftEyeCamera, _1, _2, _3)));
        engine->GetInputManager()->BindEvent<glm::mat4>(InputCode::GVR_RIGHT_EYE_MATRIX,
            InputEventDelegate<glm::mat4>(InputEvent::ON_ACTIVE, std::bind(&Camera::OnUpdateTransform, rightEyeCamera, _1, _2, _3)));

        cameras.push_back(std::unique_ptr<Camera>(std::move(leftEyeCamera)));
        cameras.push_back(std::unique_ptr<Camera>(std::move(rightEyeCamera)));
    }
}

//void World::AddCamera(Camera* camera)
//{
//    std::pair<std::set<Camera*>::iterator, bool> result = cameras.insert(camera);
//
//    if (!result.second)
//    {
//        VK_LOG(LogWorld, Warning, "Camera already added, skipping...");
//    }
//}
