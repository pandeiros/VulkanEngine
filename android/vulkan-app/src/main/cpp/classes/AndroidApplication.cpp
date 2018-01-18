#include "AndroidApplication.h"

#include <AndroidUtils.h>
#include <Core.h>
#include <World.h>
#include <Primitive.h>
#include <Rendering/ShaderTools.h>

#include <cmath>

VK_DECLARE_LOG_CATEGORY(LogApplication);

void AndroidApplication::Init()
{
    vulkan::Application::Init();

    vulkan::Device* device = instance->GetDevice();

    commandPool.Create(device->GetVkDevice(), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                       instance->GetDevice()->GetPhysicalDevice()->GetGraphicsQueueFamilyIndex());
    commandPool.AllocateCommandBuffer(device->GetVkDevice(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    device->CreateSemaphore(&semaphoreRenderComplete);

    float fov = 45.f;
    float aspect = instance->GetWindowRef().GetSurfaceSize().width / instance->GetWindowRef().GetSurfaceSize().height;
    if (aspect > 1.f)
    {
        fov *= 1.f / aspect;
    }
    camera = new vulkan::Camera(fov, aspect, 0.1f, 100.f,
                                {glm::vec3(-5, 3, -10), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0)}, vulkan::Camera::DEFAULT_CLIP_MATRIX);
    vulkan::Engine::GetEngine()->GetWorld()->AddCamera(camera);
    glm::mat4 vpMatrix = camera->GetViewProjectionMatrix();

    glm::mat4 mvpMatrix = vpMatrix * glm::mat4(1.0f);

    vulkan::Renderer* renderer = vulkan::Engine::GetEngine()->GetRenderer();

    vulkan::Buffer& uniformBuffer = renderer->GetUniformBuffer();
    uniformBuffer.CreateExclusive(device->GetVkDevice(), 0, sizeof(mvpMatrix), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    uniformBuffer.Allocate(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uniformBuffer.Copy(device->GetVkDevice(), &mvpMatrix, 0, sizeof(mvpMatrix));
    uniformBuffer.UpdateDescriptorInfo(0, sizeof(mvpMatrix));

    renderer->CreateDescriptorSetLayout(device->GetVkDevice());
    renderer->CreatePipelineLayout(device->GetVkDevice());
    renderer->InitShaders(device->GetVkDevice(), vulkan::VULKAN_VERTEX_SHADER_TEXT, vulkan::VULKAN_FRAGMENT_SHADER_TEXT);

    vulkan::Cube cube {1.f};
    uint32_t size, stride;
    void* data = cube.GetData(size, stride);

    vulkan::Buffer& vertexBuffer = renderer->GetVertexBuffer();
    vertexBuffer.CreateExclusive(device->GetVkDevice(), 0, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    vertexBuffer.Allocate(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vertexBuffer.Copy(device->GetVkDevice(), data, 0, size);
    vertexBuffer.UpdateDescriptorInfo(0, size);

    renderer->AddVertexInputBinding(0, stride, VK_VERTEX_INPUT_RATE_VERTEX);
    renderer->AddVertexInputAttribute(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
    // #TODO Change format when using texture.
    renderer->AddVertexInputAttribute(0, 1, VK_FORMAT_R32G32B32A32_SFLOAT, 16);

    renderer->InitDescriptorPool(device->GetVkDevice());
    renderer->InitDescriptorSet(device->GetVkDevice());
    renderer->InitPipelineCache(device->GetVkDevice());
    renderer->InitPipeline(device->GetVkDevice(), instance->GetWindowRef().GetSurfaceSize(), instance->GetWindowRef().GetRenderPass());

    timer = std::chrono::steady_clock();
    lastTime = timer.now();

    SetUpdateEnabled(true);

    LOGI(" === VULKAN APP STARTED SUCCESSFULLY! ===");
}

void AndroidApplication::Tick(float deltaTime)
{
    vulkan::Window& window = instance->GetWindowRef();
    vulkan::CommandBuffer& commandBuffer = commandPool.GetCommandBufferRef();
    vulkan::Queue& queue = instance->GetDevice()->GetQueueRef();
    vulkan::Renderer* renderer = vulkan::Engine::GetEngine()->GetRenderer();

    if (window.Update())
    {
        if (AndroidUtils::controllerApi)
        {
            controller_state.Update(*AndroidUtils::controllerApi);
        }

        std::chrono::duration<double> diff = std::chrono::duration_cast<std::chrono::duration<double>>(timer.now() - lastTime);
        if (diff.count() >= 1.0)
        {
            lastTime = timer.now();
            VK_LOG(LogApplication, Debug, "FPS: %.0f", vulkan::Engine::GetEngine()->GetFPS());
        }

        window.BeginRender();
        commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);

        VkRect2D renderArea{};
        renderArea.offset.x = 0;
        renderArea.offset.y = 0;
        renderArea.extent = window.GetSurfaceSize();

        std::vector<VkClearValue> clearValues = std::vector<VkClearValue>(2);

//        clearValues[0].color.float32[0] = std::sin(colorRotator) * 0.5f + 0.5f;
//        clearValues[0].color.float32[1] = std::sin(colorRotator + (float)PI * 2.f / 3.f) * 0.5f + 0.5f;
        clearValues[0].color.float32[0] = controller_state.GetTouchPos().x;
        clearValues[0].color.float32[1] = controller_state.GetTouchPos().y;
        clearValues[0].color.float32[2] = std::sin(colorRotator + (float)PI * 4.f / 3.f) * 0.5f + 0.5f;
        clearValues[0].color.float32[3] = 1.f;

        clearValues[1].depthStencil.depth = 0.f;
        clearValues[1].depthStencil.stencil = 0;

        commandBuffer.BeginRenderPass(window.GetRenderPass(), window.GetActiveFramebuffer(), renderArea, clearValues, VK_SUBPASS_CONTENTS_INLINE);

        renderer->BindPipeline(commandBuffer.GetVkCommandBufferRef(), VK_PIPELINE_BIND_POINT_GRAPHICS);
        renderer->BindDescriptorSets(commandBuffer.GetVkCommandBufferRef(), VK_PIPELINE_BIND_POINT_GRAPHICS);
        renderer->BindVertexBuffers(commandBuffer.GetVkCommandBufferRef(), {0});
        renderer->CommandSetViewports(commandBuffer.GetVkCommandBufferRef());
        renderer->CommandSetScissors(commandBuffer.GetVkCommandBufferRef());

        // #REFACTOR
        vkCmdDraw(commandBuffer.GetVkCommandBufferRef(), 12 * 3, 1, 0, 0);

        commandBuffer.EndRenderPass();
        commandBuffer.End();



        VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        queue.Submit(&pipelineStageFlags, {}, { commandBuffer.GetVkCommandBufferRef() }, {semaphoreRenderComplete}, VK_NULL_HANDLE);

        window.EndRender({semaphoreRenderComplete});

        colorRotator += 0.001f;
    }
    else
    {
        SetUpdateEnabled(false);
        SetPendingKill(true);
    }

    queue.WaitIdle();
}

//void AndroidApplication::Destroy()
//{
//    vulkan::Queue& queue = instance->GetDevice()->GetQueueRef();
//    queue.WaitIdle();
//
//    delete camera;
//
//    vulkan::Device* device = instance->GetDevice();
//    device->DestroySemaphore(semaphoreRenderComplete);
//
//    commandPool.Destroy(device->GetVkDevice());
//
//    vulkan::Application::Destroy();
//}

AndroidApplication::~AndroidApplication()
{
    vulkan::Queue& queue = instance->GetDevice()->GetQueueRef();
    queue.WaitIdle();

    delete camera;

    vulkan::Device* device = instance->GetDevice();
    device->DestroySemaphore(semaphoreRenderComplete);

    commandPool.Destroy(device->GetVkDevice());
}

