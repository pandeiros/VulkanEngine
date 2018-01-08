//#include "Rendering\Renderer.h"
#include "Window.h"
#include "CommandPool.h"

#include <array>
#include <chrono>

#include "Application.h"
#include "Utils/Math.h"

VULKAN_NS_USING;

#ifndef __ANDROID__

int main()
{
    Application application;
    application.Create("Vulkan Engine Test", 1, VK_MAKE_VERSION(1, 0, 2));
    application.Init();

    Instance& instance = application.GetInstanceRef();
    VkDevice device = instance.GetDeviceRef().GetVkDevice();

    CommandPool commandPool;
    commandPool.Create(device, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        instance.GetDeviceRef().GetPhysicalDevice()->GetGraphicsQueueFamilyIndex());

    commandPool.AllocateCommandBuffer(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    VkSemaphore semaphoreRenderComplete;
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphoreRenderComplete);
    }

    // Color fun.
    float colorRotator = 0.f;
    auto timer = std::chrono::steady_clock();
    auto lastTime = timer.now();
    uint64_t frameCounter = 0;
    uint64_t FPS = 0;

    Window& window = instance.GetWindowRef();
    CommandBuffer& commandBuffer = commandPool.GetCommandBufferRef();
    Queue& queue = instance.GetDeviceRef().GetQueueRef();

    while (window.Update())
    {
        ++frameCounter;
        if (lastTime + std::chrono::seconds(1) < timer.now())
        {
            lastTime = timer.now();
            FPS = frameCounter;
            frameCounter = 0;
            std::cout << "FPS: " << FPS << "\n";
        }

        window.BeginRender();
        commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);

        VkRect2D renderArea{};
        renderArea.offset.x = 0;
        renderArea.offset.y = 0;
        renderArea.extent = window.GetSurfaceSize();

        std::vector<VkClearValue> clearValues = std::vector<VkClearValue>(2);

        clearValues[1].depthStencil.depth = 0.f;
        clearValues[1].depthStencil.stencil = 0;
        clearValues[0].color.float32[0] = std::sin(colorRotator) * 0.5f + 0.5f;
        clearValues[0].color.float32[1] = std::sin(colorRotator + (float)PI * 2.f / 3.f) * 0.5f + 0.5f;
        clearValues[0].color.float32[2] = std::sin(colorRotator + (float)PI * 4.f / 3.f) * 0.5f + 0.5f;
        clearValues[0].color.float32[3] = 1.f;

        commandBuffer.BeginRenderPass(window.GetRenderPass(), window.GetActiveFramebuffer(), renderArea, clearValues, VK_SUBPASS_CONTENTS_INLINE);
        commandBuffer.EndRenderPass();

        commandBuffer.End();

        queue.Submit(nullptr, {}, { commandBuffer.GetVkCommandBufferRef() }, {semaphoreRenderComplete}, VK_NULL_HANDLE);

        window.EndRender({semaphoreRenderComplete});

        colorRotator += 0.1f;
    }

    queue.WaitIdle();

    vkDestroySemaphore(device, semaphoreRenderComplete, nullptr);

    commandPool.Destroy(device);

    application.Destroy();

    return 0;
}

#endif