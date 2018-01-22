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
    commandPool.AllocateCommandBuffer(device->GetVkDevice(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    float fov = glm::radians(45.f);
    float aspect = (float)instance->GetWindow()->GetSurfaceSize().width / (float)instance->GetWindow()->GetSurfaceSize().height;
    if (aspect > 1.f)
    {
        fov *= 1.f / aspect;
    }
    camera = new Camera(fov, aspect, 0.1f, 100.f,
    { glm::vec3(-5, 3, -10), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0) }, Camera::DEFAULT_CLIP_MATRIX);
    engine->GetWorld()->AddCamera(camera);

    glm::mat4 vpMatrix = camera->GetViewProjectionMatrix();
    glm::mat4 mvpMatrix = vpMatrix *glm::mat4(1.0f);

    Renderer* renderer = engine->GetRenderer();

    Buffer& uniformBuffer = renderer->GetUniformBuffer();
    uniformBuffer.CreateExclusive(device->GetVkDevice(), 0, sizeof(mvpMatrix), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    uniformBuffer.Allocate(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniformBuffer.Copy(device->GetVkDevice(), &mvpMatrix, 0, sizeof(mvpMatrix));
    uniformBuffer.UpdateDescriptorInfo(0, sizeof(mvpMatrix));

    renderer->CreateDescriptorSetLayout();
    renderer->CreatePipelineLayout(device->GetVkDevice());
    renderer->InitShaders(device->GetVkDevice(), VULKAN_VERTEX_SHADER_TEXT, VULKAN_FRAGMENT_SHADER_TEXT);

    Cube cube{ 1.f };
    uint32_t size, stride;
    void* data = cube.GetData(size, stride);

    Buffer& vertexBuffer = renderer->GetVertexBuffer();
    vertexBuffer.CreateExclusive(device->GetVkDevice(), 0, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    vertexBuffer.Allocate(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vertexBuffer.Copy(device->GetVkDevice(), data, 0, size);
    vertexBuffer.UpdateDescriptorInfo(0, size);

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
    Window* window = instance->GetWindow();
    CommandBuffer& commandBuffer = commandPool.GetCommandBufferRef();
    Queue& queue = instance->GetDevice()->GetQueueRef();
    Renderer* renderer = GetEngine()->GetRenderer();
    Device* device = instance->GetDevice();

    if (window->Update())
    {
        // #TODO Make Input class to handle this.
#ifdef __ANDROID__
        if (AndroidUtils::controllerApi)
        {
            controller_state.Update(*AndroidUtils::controllerApi);
        }
#endif

        std::chrono::duration<double> diff = std::chrono::duration_cast<std::chrono::duration<double>>(timer.now() - lastTime);
        if (diff.count() >= 1.0)
        {
            lastTime = timer.now();
            VK_LOG(LogTestApplication, Debug, "FPS: %.0f", GetEngine()->GetFPS());
        }

        camera->Move(glm::vec3(deltaTime * std::sin(colorRotator) * 10.f, 0.f, 0.f));

        glm::mat4 vpMatrix = camera->GetViewProjectionMatrix();
        glm::mat4 mvpMatrix = vpMatrix * glm::mat4(1.0f);
        Renderer* renderer = engine->GetRenderer();

        Buffer& uniformBuffer = renderer->GetUniformBuffer();
        uniformBuffer.Copy(device->GetVkDevice(), &mvpMatrix, 0, sizeof(mvpMatrix));

        window->BeginRender();

        commandBuffer.Reset(0);
        //commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);
        commandBuffer.Begin(0, nullptr);

        VkRect2D renderArea{};
        renderArea.offset.x = 0;
        renderArea.offset.y = 0;
        renderArea.extent = window->GetSurfaceSize();

        std::vector<VkClearValue> clearValues = std::vector<VkClearValue>(2);

        clearValues[0].color.float32[0] = 0.2f;
        clearValues[0].color.float32[1] = 0.2f;
        clearValues[0].color.float32[2] = std::sin(colorRotator + (float)PI / 2.f) * 0.5f + 0.5f;
        clearValues[0].color.float32[3] = 0.2f;

        clearValues[1].depthStencil.depth = 1.f;
        clearValues[1].depthStencil.stencil = 0;

        commandBuffer.BeginRenderPass(window->GetRenderPass(), window->GetActiveFramebuffer(), renderArea, clearValues, VK_SUBPASS_CONTENTS_INLINE);

        renderer->BindPipeline(commandBuffer.GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS);
        renderer->BindDescriptorSets(commandBuffer.GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS);
        renderer->BindVertexBuffers(commandBuffer.GetVkCommandBuffer(), { 0 });
        renderer->CommandSetViewports(commandBuffer.GetVkCommandBuffer());
        renderer->CommandSetScissors(commandBuffer.GetVkCommandBuffer());

        // #REFACTOR
        vkCmdDraw(commandBuffer.GetVkCommandBuffer(), 12 * 3, 1, 0, 0);

        commandBuffer.EndRenderPass();
        commandBuffer.End();

        // #TODO Clean this up
        VkFence fence = window->GetFence();
        vkResetFences(instance->GetDevice()->GetVkDevice(), 1, &fence);

        std::vector<VkCommandBuffer> commandBuffers = { commandBuffer.GetVkCommandBuffer() };
        std::vector<VkSemaphore> waitSemaphores = { window->GetSemaphore() };

        VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        queue.Submit(&pipelineStageFlags, waitSemaphores, commandBuffers,
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