#include "AndroidApplication.h"

#include <cmath>
#include <AndroidUtils.h>

void AndroidApplication::Init()
{
    vulkan::Application::Init();

    vulkan::Device& device = instance.GetDeviceRef();
//    VkDevice device = instance.GetDeviceRef().GetVkDevice();

    commandPool.Create(device.GetVkDevice(), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                       instance.GetDeviceRef().GetPhysicalDevice()->GetGraphicsFamilyIndex());
    commandPool.AllocateCommandBuffer(device.GetVkDevice(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    device.CreateSemaphore(&semaphoreRenderComplete);
//    {
//        VkSemaphoreCreateInfo semaphoreCreateInfo{};
//        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//        vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphoreRenderComplete);
//    }

    timer = std::chrono::steady_clock();
    lastTime = timer.now();

    SetUpdateEnabled(true);
}

void AndroidApplication::Tick()
{
    vulkan::Window& window = instance.GetWindowRef();
    vulkan::CommandBuffer& commandBuffer = commandPool.GetCommandBufferRef();
    vulkan::Queue& queue = instance.GetDeviceRef().GetQueueRef();

    if (window.Update())
    {
        if (AndroidUtils::controllerApi)
        {
            controller_state.Update(*AndroidUtils::controllerApi);
        }

        ++frameCounter;
        std::chrono::duration<double> diff = std::chrono::duration_cast<std::chrono::duration<double>>(timer.now() - lastTime);
        if (diff.count() >= 1.0)
        {
            lastTime = timer.now();
            FPS = frameCounter;
            frameCounter = 0;
//            std::cout << "FPS: " << FPS << "\n";
        }

        window.BeginRender();
        commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);

        VkRect2D renderArea{};
        renderArea.offset.x = 0;
        renderArea.offset.y = 0;
        renderArea.extent = window.GetSurfaceSize();

        std::vector<VkClearValue> clearValues = std::vector<VkClearValue>(2);

        clearValues[0].depthStencil.depth = 0.f;
        clearValues[0].depthStencil.stencil = 0;
//        clearValues[1].color.float32[0] = std::sin(colorRotator) * 0.5f + 0.5f;
//        clearValues[1].color.float32[1] = std::sin(colorRotator + (float)PI * 2.f / 3.f) * 0.5f + 0.5f;
        clearValues[1].color.float32[0] = controller_state.GetTouchPos().x;
        clearValues[1].color.float32[1] = controller_state.GetTouchPos().y;
        clearValues[1].color.float32[2] = std::sin(colorRotator + (float)PI * 4.f / 3.f) * 0.5f + 0.5f;
        clearValues[1].color.float32[3] = 1.f;

        commandBuffer.BeginRenderPass(window.GetRenderPass(), window.GetActiveFramebuffer(), renderArea, clearValues, VK_SUBPASS_CONTENTS_INLINE);
        commandBuffer.EndRenderPass();

        commandBuffer.End();

        queue.Submit(nullptr, {}, { commandBuffer.GetVkCommandBufferRef() }, {semaphoreRenderComplete}, VK_NULL_HANDLE);

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

void AndroidApplication::Destroy()
{
    vulkan::Queue& queue = instance.GetDeviceRef().GetQueueRef();
    queue.WaitIdle();

    vulkan::Device& device = instance.GetDeviceRef();
    device.DestroySemaphore(semaphoreRenderComplete);

    commandPool.Destroy(device.GetVkDevice());

    vulkan::Application::Destroy();
}
