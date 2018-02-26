/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "TestApplication.h"

#include "Core.h"
#include "World.h"
#include "Rendering/Cube.h"
#include "Rendering/ShaderTools.h"

#ifdef __ANDROID__
#include "AndroidUtils.h"
#endif

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

VK_DECLARE_LOG_CATEGORY(LogTestApplication);

VULKAN_NS_USING;

TestApplication::TestApplication()
    : Application("Vulkan Engine App Test", 1, VK_MAKE_VERSION(1, 0, 2))
{}

TestApplication::~TestApplication()
{
    Queue& queue = instance->GetDevice()->GetQueueRef();
    queue.WaitIdle();

    //delete camera;

    //Device* device = instance->GetDevice();

    //commandPool.Destroy(device->GetVkDevice());
}

void TestApplication::Init()
{
    VK_PERFORMANCE_DATA("Application initialization");

    Application::Init();

    Device* device = instance->GetDevice();

    //commandPool.Create(device->GetVkDevice(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    //    instance->GetDevice()->GetPhysicalDevice()->GetGraphicsQueueFamilyIndex());

    //commandPool.AllocateCommandBuffers(device->GetVkDevice(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VULKAN_COMMAND_BUFFER_COUNT);

    //float fov = glm::radians(90.f);
    //float aspect = (float)instance->GetWindow()->GetSurfaceSize().width / (float)instance->GetWindow()->GetSurfaceSize().height;
    //if (aspect > 1.f)
    //{
    //    fov *= 1.f / aspect;
    //}
    //camera = new Camera(fov, aspect, 0.1f, 100.f,
    //{ glm::vec3(0, 50, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0) }, Camera::DEFAULT_CLIP_MATRIX);

#ifdef __ANDROID__
    engine->GetWorld()->SetCamera(CameraMode::VR, 90.f, instance->GetWindow()->GetAspectRatio(), 0.1f, 200.f);
#else
    engine->GetWorld()->SetCamera(CameraMode::DEFAULT, 90.f, instance->GetWindow()->GetAspectRatio(), 0.1f, 200.f);
#endif

    //engine->GetWorld()->SetLensUndistortionCoefficients(glm::vec4(0.2, 0.24, 0.0, 0.0));
    engine->GetRenderer()->Init();



    //glm::mat4 vpMatrix = camera->GetViewProjectionMatrix();
    //glm::mat4 mvpMatrix = vpMatrix * glm::mat4(1.0f);

    //Renderer* renderer = engine->GetRenderer();

    //for (uint32_t i = 0; i < VULKAN_DESCRIPTOR_SETS_COUNT; ++i)
    //{
    //    Buffer& uniformBuffer = renderer->GetUniformBuffer(i);
    //    uniformBuffer.CreateExclusive(device->GetVkDevice(), 0, 2 * 256, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT); // #TODO Use device properties instead of 256
    //    uniformBuffer.Allocate(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    //    uniformBuffer.UpdateDescriptorInfo(0, VK_WHOLE_SIZE);
    //}

    //renderer->CreateDescriptorSetLayout();
    //renderer->CreatePipelineLayout();
    //renderer->InitShaders(VULKAN_VERTEX_SHADER_TEXT, VULKAN_FRAGMENT_SHADER_TEXT);

    {
        VK_PERFORMANCE_SECTION("Scene initialization");

        // Cubes
        {
            RenderComponent* renderComponent = engine->GetRenderer()->AddRenderComponent(Cube::GetCubeVertexData(), Cube::GetCubeShaderEntry());

            //uint32_t finalSize, size, stride;
            //std::vector<Cube> cubes;
            int32_t cubeCount = 1;

            for (int32_t i = 0; i < cubeCount; ++i)
            {
                for (int32_t j = 0; j < cubeCount; ++j)
                {
                    //cubes.push_back(Cube{ 1.f, { glm::vec3(std::cos(PI / 2 * (i)) * 7, 0.f, std::sin(PI / 2 * i) * 7) } });

                    float xArg = float(i - cubeCount / 2);
                    float yArg = float(j - cubeCount / 2);
                    float y = 10 * ((float)std::cos(xArg / cubeCount * PI * 2) + (float)std::sin(yArg * 3 / 2 / cubeCount * PI * 2));

                    Actor* actor = new Actor;
                    SceneComponent* sceneComponent = new SceneComponent;
                    actor->SetSceneComponent(sceneComponent);
                    sceneComponent->SetRenderComponent(renderComponent);

                    actor->SetTransform({ glm::vec3(xArg * 0.75f, y, yArg * 0.75f), glm::vec3(0.25f, 0.25f, 0.25f) });
                    actor->GetSceneComponent()->SetColor({ xArg / cubeCount + 0.5f, 0, yArg / cubeCount + 0.5f });

                    //Cube* cube = new Cube(1.f, { glm::vec3(xArg * 0.75f, y, yArg * 0.75f), glm::vec3(0.25f, 0.25f, 0.25f) });
                    //cube->SetColor({ xArg / cubeCount + 0.5f, 0, yArg / cubeCount + 0.5f });
                    //actor->SetRenderComponent(cube);

                    engine->GetWorld()->AddActor(actor);

                    //cubes.push_back(Cube{ 1.f, { glm::vec3(xArg * 0.75f, y, yArg * 0.75f), glm::vec3(0.25f, 0.25f, 0.25f)} });
                    //cubes[cubes.size() - 1].SetColor({ xArg / cubeCount + 0.5f, 0, yArg / cubeCount + 0.5f});
                }
            }
        }

        // OBJ Loading
        {
            VertexData vertices;
            //std::vector<uint32_t> indices;

            // #TODO Refactor
#ifdef __ANDROID__
            const std::string MODEL_PATH = "minicooper.obj";
#else
            const std::string MODEL_PATH = "data/minicooper.obj";
#endif
            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string err;

#ifdef __ANDROID__
            // #TODO Change name of this method
            std::vector<char> buffer = AndroidUtils::GetFileStream(MODEL_PATH);
            wrap_vector_as_istream databuf(buffer);
            std::istream is(&databuf);
            if (tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &is))
#else
            if (tinyobj::LoadObj(&attrib, &shapes, &materials, &err, MODEL_PATH.c_str()))
#endif
            {
                for (const auto &shape : shapes)
                {
                    for (const auto &indice : shape.mesh.indices)
                    {
                        Vertex vertex = {};

                        vertex.x = attrib.vertices[3 * indice.vertex_index + 0];
                        vertex.y = attrib.vertices[3 * indice.vertex_index + 1];
                        vertex.z = attrib.vertices[3 * indice.vertex_index + 2];
                        vertex.w = 1.f;

                        vertex.r = vertex.g = vertex.b = 0.8f;
                        vertex.a = 1.f;

                        vertices.push_back(vertex);
                        //indices.push_back(indices.size());
                    }
                }

                VK_LOG(LogTestApplication, Info, "Loaded model has %d vertices.", (uint32_t)vertices.size());

                RenderComponent *objRenderComponent = engine->GetRenderer()->AddRenderComponent(
                        vertices, Cube::GetCubeShaderEntry());
                Actor *actor = new Actor;
                SceneComponent *sceneComponent = new SceneComponent;
                actor->SetSceneComponent(sceneComponent);
                sceneComponent->SetRenderComponent(objRenderComponent);
                actor->GetSceneComponent()->SetColor({1.f, 1.f, 1.f});
//                actor->SetTransform({glm::vec3(0.f), glm::vec3(5.f), glm::vec3(glm::radians(-90.f), glm::radians(90.f), 0.f)}); // House
                actor->SetTransform({glm::vec3(0.f), glm::vec3(0.1f), glm::vec3(glm::radians(-90.f), glm::radians(-75.f), 0.f)}); // Minicooper
//                actor->SetTransform({glm::vec3(0.f), glm::vec3(1.f), glm::vec3(0.f)}); // Teapot
                engine->GetWorld()->AddActor(actor);
            }
            else
            {
                VK_LOG(LogTestApplication, Error, "Cannot load OBJ file!\n%s", err.c_str());
            }
        }
    }

    //cubesToDraw = cubeCount * cubeCount;
    //cubes[0].GetData(size, stride);
    //finalSize = size * cubesToDraw;

    //Buffer& vertexBuffer = renderer->GetVertexBuffer();
    //vertexBuffer.CreateExclusive(device->GetVkDevice(), 0, finalSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    //vertexBuffer.Allocate(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    //uint32_t offset = 0;
    //for (uint32_t i = 0; i < cubes.size(); ++i)
    //{
    //    cubes[i].UpdateVertices();
    //    void* data = cubes[i].GetData(size, stride);
    //    vertexBuffer.Copy(device->GetVkDevice(), data, offset, size);
    //    offset += size;
    //}
    //vertexBuffer.UpdateDescriptorInfo(0, VK_WHOLE_SIZE);

    //renderer->AddVertexInputBinding(0, stride, VK_VERTEX_INPUT_RATE_VERTEX);
    //renderer->AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
    //// #TODO Change format when using texture.
    //renderer->AddVertexInputAttribute(1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 16);

    //renderer->InitDescriptorPool(device->GetVkDevice());
    //renderer->InitDescriptorSet(device->GetVkDevice());
    //renderer->InitPipelineCache(device->GetVkDevice());
    //renderer->InitPipeline(device->GetVkDevice(), instance->GetWindow()->GetSurfaceSize(), instance->GetWindow()->GetRenderPass());

    timer = std::chrono::steady_clock();
    lastTime = timer.now();

    SetUpdateEnabled(true);

    VK_LOG(LogTestApplication, Info, "VULKAN APP INITIALIZED");
}

void TestApplication::Tick(float deltaTime)
{
    std::chrono::duration<double> diff = std::chrono::duration_cast<std::chrono::duration<double>>(timer.now() - lastTime);
    if (diff.count() >= 1.0)
    {
        lastTime = timer.now();
        //VK_LOG(LogTestApplication, Debug, "FPS: %.0f", GetEngine()->GetFPS());
        GetEngine()->RequestPerformanceDataLog();
    }

    return;

    //////////////////////////////////////////////////////////////////////////

    VK_PERFORMANCE_SECTION("Test application");

    Window* window = instance->GetWindow();
    std::vector<CommandBuffer>& commandBuffers = commandPool.GetCommandBuffers();
    Queue& queue = instance->GetDevice()->GetQueueRef();
    Renderer* renderer = GetEngine()->GetRenderer();
    Device* device = instance->GetDevice();

    if (window->Update())
    {
        glm::mat4 headMatrix = glm::translate(glm::rotate(GetEngine()->GetInputManager()->GetHeadMatrix(), glm::radians(180.f), glm::vec3(0.f, 0.f, 1.f)), glm::vec3(0.f, 0.f, -10.f));

        std::chrono::duration<double> diff = std::chrono::duration_cast<std::chrono::duration<double>>(timer.now() - lastTime);
        if (diff.count() >= 1.0)
        {
            lastTime = timer.now();
            VK_LOG(LogTestApplication, Debug, "FPS: %.0f", GetEngine()->GetFPS());
            //VK_LOG(LogTestApplication, Debug, "View: %s", glm::to_string(camera->GetViewMatrix()).c_str());
            //VK_LOG(LogTestApplication, Debug, "Head: %s", glm::to_string(glm::translate(glm::rotate(GetEngine()->GetInputManager()->GetHeadMatrix(), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f)), glm::vec3(0.f, 0.f, -10.f))).c_str());
            //VK_LOG(LogTestApplication, Debug, "ViewportMAtrix: %s", glm::to_string(glm::translate(glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f)), glm::vec3(0, 0, 0.f))).c_str());
            //VK_LOG(LogTestApplication, Debug, "1.f matrix: %s", glm::to_string(glm::mat4(1.f)).c_str());
        }

        //camera->Move(glm::vec3(deltaTime * std::sin(colorRotator) * 10.f, 0.f, 0.f));

        //glm::mat4 vpMatrix = camera->GetViewProjectionMatrix();
        //glm::mat4 mvpMatrix = vpMatrix * glm::mat4(1.0f);

        //glm::mat4 mvpMatrix = camera->GetClipMatrix() * camera->GetProjectionMatrix() * GetEngine()->GetInputManager()->GetHeadMatrix() * glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 1.f, 1.f)), glm::vec3(0, 0, -10));
        //Renderer* renderer = engine->GetRenderer();

        //Buffer& uniformBuffer = renderer->GetUniformBuffer();
        //uniformBuffer.Copy(device->GetVkDevice(), &mvpMatrix, 0, sizeof(mvpMatrix));

        window->BeginRender();

        std::vector<uint32_t> commandBufferIndexes = {};
        for (uint32_t i = 0; i < commandBuffers.size(); ++i)
        {
            commandBufferIndexes.push_back(i);

            VkRect2D renderArea{};
            renderArea.offset.x = i * window->GetSurfaceSize().width / 2;
            renderArea.offset.y = 0;
            renderArea.extent = window->GetSurfaceSize();
            renderArea.extent.width /= 2;

            //glm::mat4 viewportMatrix = glm::translate(glm::scale(glm::mat4(1.f), glm::vec3(renderArea.extent.width / 2, renderArea.extent.height / 2, 1.f - 0.f)), glm::vec3(renderArea.offset.x, renderArea.offset.y, 0.f));
            //glm::mat4 viewportMatrix = glm::translate(glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f)), glm::vec3(renderArea.offset.x, renderArea.offset.y, 0.f));
            //glm::mat4 viewportMatrix = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(renderArea.offset.x, renderArea.offset.y, 0.f)), glm::vec3(1.f, 1.f, 1.f));

            glm::mat4 testTransform = glm::translate(glm::mat4(1.f), glm::vec3(i == 0 ? -0.5f : 0.5f, 0.f, 0.f));
#ifdef __ANDROID__
            glm::mat4 mvpMatrix = camera->GetClipMatrix() * testTransform * camera->GetProjectionMatrix() *
                (i == 0 ? GetEngine()->GetInputManager()->GetLeftEyeMatrix() : GetEngine()->GetInputManager()->GetRightEyeMatrix()) * glm::translate(glm::mat4(1.0f), glm::vec3(0, -10, 10));
#else
            glm::mat4 mvpMatrix = camera->GetClipMatrix() * testTransform * camera->GetProjectionMatrix() * camera->GetViewMatrix() * glm::mat4(1.0f);
#endif
            Renderer* renderer = engine->GetRenderer();

            Buffer& uniformBuffer = renderer->GetUniformBuffer(0);
            uniformBuffer.Copy(device->GetVkDevice(), &mvpMatrix, i * 256, sizeof(mvpMatrix));
            //uniformBuffer.UpdateDescriptorInfo(i * 256, 64);

            commandBuffers[i].Reset(0);
            //commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);
            commandBuffers[i].Begin(0, nullptr);

            std::vector<VkClearValue> clearValues = std::vector<VkClearValue>(2);

            clearValues[0].color.float32[0] = 0.f;
            clearValues[0].color.float32[1] = 0.f;
            clearValues[0].color.float32[2] = 0.f;
            clearValues[0].color.float32[3] = 0.f;

            clearValues[1].depthStencil.depth = 1.f;
            clearValues[1].depthStencil.stencil = 0;

            //renderer->UpdateDescriptorSets(device->GetVkDevice(), i);

            {
                VK_PERFORMANCE_SECTION("Render initialization");
                uint32_t dynamicOffset = i * 256;
                commandBuffers[i].BeginRenderPass(window->GetRenderPass(), window->GetActiveFramebuffer(), renderArea, clearValues, VK_SUBPASS_CONTENTS_INLINE);
                renderer->BindPipeline(commandBuffers[i].GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS);
                renderer->BindDescriptorSets(commandBuffers[i].GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, { dynamicOffset });
                renderer->BindVertexBuffers(commandBuffers[i].GetVkCommandBuffer(), { 0 });
                renderer->CommandSetViewports(commandBuffers[i].GetVkCommandBuffer());
                renderer->CommandSetScissors(commandBuffers[i].GetVkCommandBuffer(), i);
            }

            // #REFACTOR
            {
                VK_PERFORMANCE_SECTION("Draw");
                vkCmdDraw(commandBuffers[i].GetVkCommandBuffer(), 12 * 3 * cubesToDraw, 1, 0, 0);
            }

            commandBuffers[i].EndRenderPass();
            commandBuffers[i].End();
        }

        // #TODO Clean this up
        VkFence fence = window->GetFence();
        vkResetFences(instance->GetDevice()->GetVkDevice(), 1, &fence);

        //std::vector<VkCommandBuffer> commandBuffers = { commandBuffer.GetVkCommandBuffer() };
        std::vector<VkSemaphore> waitSemaphores = { window->GetSemaphore() };

        VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        queue.Submit(&pipelineStageFlags, waitSemaphores, commandPool.GetVkCommandBuffers(commandBufferIndexes),
        {}, window->GetFence());

        window->EndRender({}, { window->GetFence() });

        colorRotator += deltaTime;
    }
    else
    {
        SetUpdateEnabled(false);
        SetPendingKill(true);
    }

    queue.WaitIdle();
}