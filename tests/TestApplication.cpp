/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "TestApplication.h"

#include "Core.h"
#include "World.h"
#include "Primitive.h"
#include "Rendering/ShaderTools.h"

#ifdef __ANDROID__
#include "AndroidUtils.h"
#endif

VK_DECLARE_LOG_CATEGORY(LogTestApplication);

VULKAN_NS_USING;

TestApplication::TestApplication()
    : Application("Vulkan Engine App Test", 1, VK_MAKE_VERSION(1, 0, 2))
{}

TestApplication::~TestApplication()
{
    Queue& queue = instance->GetDevice()->GetQueueRef();
    queue.WaitIdle();

    delete camera;

    Device* device = instance->GetDevice();

    commandPool.Destroy(device->GetVkDevice());
}

void TestApplication::Init()
{
    Application::Init();

    Device* device = instance->GetDevice();

    commandPool.Create(device->GetVkDevice(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        instance->GetDevice()->GetPhysicalDevice()->GetGraphicsQueueFamilyIndex());

    commandPool.AllocateCommandBuffers(device->GetVkDevice(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, VULKAN_COMMAND_BUFFER_COUNT);

    float fov = glm::radians(90.f);
    float aspect = (float)instance->GetWindow()->GetSurfaceSize().width / (float)instance->GetWindow()->GetSurfaceSize().height;
    if (aspect > 1.f)
    {
        fov *= 1.f / aspect;
    }
    camera = new Camera(fov, aspect, 0.1f, 100.f,
    { glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0) }, Camera::DEFAULT_CLIP_MATRIX);
    engine->GetWorld()->AddCamera(camera);

    glm::mat4 vpMatrix = camera->GetViewProjectionMatrix();
    glm::mat4 mvpMatrix = vpMatrix * glm::mat4(1.0f);

    Renderer* renderer = engine->GetRenderer();

    for (uint32_t i = 0; i < VULKAN_DESCRIPTOR_SETS_COUNT; ++i)
    {
        Buffer& uniformBuffer = renderer->GetUniformBuffer(i);
        uniformBuffer.CreateExclusive(device->GetVkDevice(), 0, 2 * 256, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT); // #TODO Use device properties instead of 256
        uniformBuffer.Allocate(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        //uniformBuffer.Copy(device->GetVkDevice(), &mvpMatrix, 0, sizeof(mvpMatrix));
        uniformBuffer.UpdateDescriptorInfo(0, VK_WHOLE_SIZE);
    }

    renderer->CreateDescriptorSetLayout();
    renderer->CreatePipelineLayout(device->GetVkDevice());
    renderer->InitShaders(device->GetVkDevice(), VULKAN_VERTEX_SHADER_TEXT, VULKAN_FRAGMENT_SHADER_TEXT);


    //Cube cube{ 1.f, };
    uint32_t finalSize, size, stride;
    std::vector<Cube> cubes;
    for (uint32_t i = 0; i < 4; ++i)
    {
        cubes.push_back(Cube { 1.f, {glm::vec3(std::cos(PI / 2 * (i)) * 7, 0.f, std::sin(PI / 2 * i) * 7)} });
    }
    cubes[0].GetData(size, stride);
    finalSize = size * (uint32_t)cubes.size();

    Buffer& vertexBuffer = renderer->GetVertexBuffer();
    vertexBuffer.CreateExclusive(device->GetVkDevice(), 0, finalSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    vertexBuffer.Allocate(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    uint32_t offset = 0;
    for (uint32_t i = 0; i < cubes.size(); ++i)
    {
        cubes[i].UpdateVertices();
        void* data = cubes[i].GetData(size, stride);
        vertexBuffer.Copy(device->GetVkDevice(), data, offset, size);
        offset += size;
    }
    vertexBuffer.UpdateDescriptorInfo(0, VK_WHOLE_SIZE);

    renderer->AddVertexInputBinding(0, stride, VK_VERTEX_INPUT_RATE_VERTEX);
    renderer->AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
    // #TODO Change format when using texture.
    renderer->AddVertexInputAttribute(1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 16);

    renderer->InitDescriptorPool(device->GetVkDevice());
    renderer->InitDescriptorSet(device->GetVkDevice());
    renderer->InitPipelineCache(device->GetVkDevice());
    renderer->InitPipeline(device->GetVkDevice(), instance->GetWindow()->GetSurfaceSize(), instance->GetWindow()->GetRenderPass());

    timer = std::chrono::steady_clock();
    lastTime = timer.now();

    SetUpdateEnabled(true);

    VK_LOG(LogTestApplication, Info, "VULKAN APP INITIALIZED");
}

void TestApplication::Tick(float deltaTime)
{
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

        for (uint32_t i = 0; i < commandBuffers.size(); ++i)
        {
            VkRect2D renderArea{};
            renderArea.offset.x = i * window->GetSurfaceSize().width / 2;
            renderArea.offset.y = 0;
            renderArea.extent = window->GetSurfaceSize();
            renderArea.extent.width /= 2;

            //glm::mat4 viewportMatrix = glm::translate(glm::scale(glm::mat4(1.f), glm::vec3(renderArea.extent.width / 2, renderArea.extent.height / 2, 1.f - 0.f)), glm::vec3(renderArea.offset.x, renderArea.offset.y, 0.f));
            //glm::mat4 viewportMatrix = glm::translate(glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f)), glm::vec3(renderArea.offset.x, renderArea.offset.y, 0.f));
            //glm::mat4 viewportMatrix = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(renderArea.offset.x, renderArea.offset.y, 0.f)), glm::vec3(1.f, 1.f, 1.f));

            glm::mat4 testTransform = glm::translate(glm::mat4(1.f), glm::vec3(i == 0 ? -0.5f : 0.5f, 0.f, 0.f));
            glm::mat4 mvpMatrix = camera->GetClipMatrix() * testTransform * camera->GetProjectionMatrix() * GetEngine()->GetInputManager()->GetHeadMatrix() * glm::translate(glm::mat4(1.0f), glm::vec3(0, -3, 0));
            //glm::mat4 mvpMatrix = camera->GetClipMatrix() * testTransform * camera->GetProjectionMatrix() * camera->GetViewMatrix() * glm::translate(glm::mat4(1.0f), glm::vec3(0, -3, -20));
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
                vkCmdDraw(commandBuffers[i].GetVkCommandBuffer(), 12 * 3 * 4, 1, 0, 0);
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
        queue.Submit(&pipelineStageFlags, waitSemaphores, commandPool.GetVkCommandBuffers({0, 1}),
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