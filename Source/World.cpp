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
    SetUpdateEnabled(true);
}

World::~World()
{
    cameras.clear();

    for (Actor* actor : actors)
    {
        delete actor;
    }

    actors.clear();
}

void World::Tick(float deltaTime)
{
    VK_PERFORMANCE_SECTION("World update");

    for (Actor* actor : actors)
    {
        if (actor->IsDirty())
        {
            SetDirty(true);
        }

        actor->Update(deltaTime);
    }
}

bool World::PrepareVertexData(VertexBuffer& vertexBuffer, ShaderIndexData& shaderIndexData)
{
    if (actors.size() == 0)
    {
        return false;
    }

    RenderComponent* renderComponent = actors[0]->GetSceneComponent()->GetRenderComponent();
    VK_ASSERT(renderComponent, "Invalid render component!");

    uint32_t size, stride;
    void* data = renderComponent->GetData(size, stride);
    shaderIndexData = renderComponent->GetShaderIndexData();

    VkDeviceSize bufferSize = size * actors.size();
    vertexBuffer.CreateExclusive(device->GetVkDevice(), 0, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    vertexBuffer.Allocate(device.get(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vertexBuffer.SetStride(stride);

    uint32_t offset = 0;
    for (uint32_t i = 0; i < actors.size(); ++i)
    {
        SceneComponent* sceneComponent = actors[i]->GetSceneComponent();
        vertexBuffer.Copy(device->GetVkDevice(), data, offset, size, sceneComponent ? sceneComponent->GetTransform() : actors[i]->GetTransform());
        offset += size;
    }

    vertexBuffer.UpdateDescriptorInfo(0, VK_WHOLE_SIZE);

    return true;
}

glm::mat4 World::GetCameraMatrix(uint32_t viewportIndex)
{
    return cameras[viewportIndex]->GetCameraMatrix();
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
        Camera* camera = new Camera(CameraMode::DEFAULT, fov, aspectRatio, 0.1f, 100.f,
        { glm::vec3(-2, 10, 10), glm::vec3(0, 0.7, 0.7), glm::vec3(0, 0.7, -0.7) }, Camera::DEFAULT_CLIP_MATRIX);

        camera->SetViewportMatrix(glm::mat4(1.f));

        cameras.push_back(std::unique_ptr<Camera>(std::move(camera)));
    }
    else if (cameraMode == CameraMode::VR)
    {
        Camera* leftEyeCamera = new Camera(CameraMode::VR, fov, aspectRatio, 0.1f, 100.f,
        { glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0) });
        Camera* rightEyeCamera = new Camera(CameraMode::VR, fov, aspectRatio, 0.1f, 100.f,
        { glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0) });

        engine->GetInputManager()->BindEvent<glm::mat4>(InputCode::GVR_LEFT_EYE_MATRIX,
            InputEventDelegate<glm::mat4>(InputEvent::ON_ACTIVE, std::bind(&Camera::OnUpdateTransform, leftEyeCamera, _1, _2, _3)));
        engine->GetInputManager()->BindEvent<glm::mat4>(InputCode::GVR_RIGHT_EYE_MATRIX,
            InputEventDelegate<glm::mat4>(InputEvent::ON_ACTIVE, std::bind(&Camera::OnUpdateTransform, rightEyeCamera, _1, _2, _3)));

        leftEyeCamera->SetViewportMatrix(glm::translate(glm::mat4(1.f), glm::vec3(-0.5f, 0.f, 0.f)));
        rightEyeCamera->SetViewportMatrix(glm::translate(glm::mat4(1.f), glm::vec3(0.5f, 0.f, 0.f)));

        cameras.push_back(std::unique_ptr<Camera>(std::move(leftEyeCamera)));
        cameras.push_back(std::unique_ptr<Camera>(std::move(rightEyeCamera)));
    }
}

void World::AddActor(Actor* actor)
{
    actors.push_back(actor);
    actor->SetDirty(true);
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
