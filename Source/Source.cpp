#include "Rendering\Renderer.h"
#include "Rendering\Window.h"

#include <array>
#include <chrono>

int main()
{
    vulkan::Renderer R;

    vulkan::Window* Win = R.OpenWindow(800, 600, "Vulkan Engine");

    auto Device = R.GetDevice();
    auto Queue = R.GetQueue();

    //VkFenceCreateInfo FenceCreateInfo {};
    //FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    //VkFence Fence;
    //vkCreateFence(Device, &FenceCreateInfo, nullptr, &Fence);

    //VkSemaphore Semaphore;
    //VkSemaphoreCreateInfo SemaphoreCreateInfo {};
    //SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    //vkCreateSemaphore(Device, &SemaphoreCreateInfo, nullptr, &Semaphore);

    VkCommandPool CommandPool = VK_NULL_HANDLE;
    VkCommandPoolCreateInfo CommandPoolInfo {};
    CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolInfo.queueFamilyIndex = R.GetGraphicsFamilyIndex();
    CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vkCreateCommandPool(Device, &CommandPoolInfo, nullptr, &CommandPool);

    VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
    VkCommandBufferAllocateInfo CommandBufferAllocateInfo {};
    CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    CommandBufferAllocateInfo.commandPool = CommandPool;
    CommandBufferAllocateInfo.commandBufferCount = 1;
    CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    vulkan::ErrorCheck(vkAllocateCommandBuffers(Device, &CommandBufferAllocateInfo, &CommandBuffer));

    VkSemaphore RenderCompleteSemaphore;
    VkSemaphoreCreateInfo SemaphoreCreateInfo {};
    SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(Device, &SemaphoreCreateInfo, nullptr, &RenderCompleteSemaphore);

    // Color fun.
    float ColorRotator = 0.f;
    auto Timer = std::chrono::steady_clock();
    auto LastTime = Timer.now();
    uint64_t FrameCounter = 0;
    uint64_t FPS = 0;

    while (R.Run())
    {
        ++FrameCounter;
        if (LastTime + std::chrono::seconds(1) < Timer.now())
        {
            LastTime = Timer.now();
            FPS = FrameCounter;
            FrameCounter = 0;
            std::cout << "FPS: " << FPS << "\n";
        }

        Win->BeginRender();

        VkCommandBufferBeginInfo BeginInfo {};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

        VkRect2D RenderArea{};
        RenderArea.offset.x = 0;
        RenderArea.offset.y = 0;
        RenderArea.extent = Win->GetSurfaceSize();

        std::array<VkClearValue, 2> ClearValues{};
        ClearValues[0].depthStencil.depth = 0.f;
        ClearValues[0].depthStencil.stencil = 0;
        ClearValues[1].color.float32[0] = std::sin(ColorRotator) * 0.5f + 0.5f;
        ClearValues[1].color.float32[1] = std::sin(ColorRotator + PI * 2.f / 3.f) * 0.5f + 0.5f;
        ClearValues[1].color.float32[2] = std::sin(ColorRotator + PI * 4.f / 3.f) * 0.5f + 0.5f;
        ClearValues[1].color.float32[3] = 1.f;

        VkRenderPassBeginInfo RenderPassBeginInfo{};
        RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassBeginInfo.renderPass = Win->GetRenderPass();
        RenderPassBeginInfo.framebuffer = Win->GetActiveFramebuffer();
        RenderPassBeginInfo.renderArea = RenderArea;
        RenderPassBeginInfo.clearValueCount = ClearValues.size();
        RenderPassBeginInfo.pClearValues = ClearValues.data();

        vkCmdBeginRenderPass(CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdEndRenderPass(CommandBuffer);

        vkEndCommandBuffer(CommandBuffer);

        VkSubmitInfo SubmitInfo {};
        SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.waitSemaphoreCount = 0;
        SubmitInfo.pWaitSemaphores = nullptr;
        SubmitInfo.pWaitDstStageMask = nullptr;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers = &CommandBuffer;
        SubmitInfo.signalSemaphoreCount = 1;
        SubmitInfo.pSignalSemaphores = &RenderCompleteSemaphore;

        vkQueueSubmit(Queue, 1, &SubmitInfo, VK_NULL_HANDLE);

        Win->EndRender({RenderCompleteSemaphore});

        ColorRotator += 0.001f;
    }

    vkQueueWaitIdle(Queue);

    vkDestroySemaphore(Device, RenderCompleteSemaphore, nullptr);

    vkDestroyCommandPool(Device, CommandPool, nullptr);

    //auto Device = R.Device;
    //auto Queue = R.Queue;

    //VkFenceCreateInfo FenceCreateInfo {};
    //FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    //VkFence Fence;
    //vkCreateFence(Device, &FenceCreateInfo, nullptr, &Fence);

    //VkSemaphore Semaphore;
    //VkSemaphoreCreateInfo SemaphoreCreateInfo {};
    //SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    //vkCreateSemaphore(Device, &SemaphoreCreateInfo, nullptr, &Semaphore);

    //VkCommandPool CommandPool;

    //VkCommandPoolCreateInfo CommandPoolInfo {};
    //CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    //CommandPoolInfo.queueFamilyIndex = R.GraphicsFamilyIndex;
    //CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    //vkCreateCommandPool(Device, &CommandPoolInfo, nullptr, &CommandPool);

    //VkCommandBuffer CommandBuffers[2];
    //VkCommandBufferAllocateInfo CommandBufferAllocateInfo {};
    //CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    //CommandBufferAllocateInfo.commandPool = CommandPool;
    //CommandBufferAllocateInfo.commandBufferCount = 2;
    //CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    //vkAllocateCommandBuffers(Device, &CommandBufferAllocateInfo, CommandBuffers);

    //{
    //	VkCommandBufferBeginInfo BeginInfo {};
    //	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //	vkBeginCommandBuffer(CommandBuffers[0], &BeginInfo);

    //	vkCmdPipelineBarrier(CommandBuffers[0],
    //						 VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
    //						 VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
    //						 0,
    //						 0, nullptr,
    //						 0, nullptr,
    //						 0, nullptr);

    //	VkViewport Viewport {};
    //	Viewport.maxDepth = 1.f;
    //	Viewport.minDepth = 0.f;
    //	Viewport.width = 512;
    //	Viewport.height = 512;
    //	Viewport.x = 0;
    //	Viewport.y = 0;
    //	vkCmdSetViewport(CommandBuffers[0], 0, 1, &Viewport);

    //	vkEndCommandBuffer(CommandBuffers[0]);
    //}
    //{
    //	VkCommandBufferBeginInfo BeginInfo {};
    //	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //	vkBeginCommandBuffer(CommandBuffers[1], &BeginInfo);

    //	VkViewport Viewport {};
    //	Viewport.maxDepth = 1.f;
    //	Viewport.minDepth = 0.f;
    //	Viewport.width = 512;
    //	Viewport.height = 512;
    //	Viewport.x = 0;
    //	Viewport.y = 0;
    //	vkCmdSetViewport(CommandBuffers[1], 0, 1, &Viewport);

    //	vkEndCommandBuffer(CommandBuffers[1]);
    //}
    //{
    //	VkSubmitInfo SubmitInfo {};
    //	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //	SubmitInfo.commandBufferCount = 1;
    //	SubmitInfo.pCommandBuffers = &CommandBuffers[0];
    //	SubmitInfo.signalSemaphoreCount = 1;
    //	SubmitInfo.pSignalSemaphores = &Semaphore;
    //	vkQueueSubmit(Queue, 1, &SubmitInfo, VK_NULL_HANDLE);
    //}
    //{
    //	VkPipelineStageFlags Flags[] {VK_PIPELINE_STAGE_ALL_COMMANDS_BIT};
    //	VkSubmitInfo SubmitInfo {};
    //	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //	SubmitInfo.commandBufferCount = 1;
    //	SubmitInfo.pCommandBuffers = &CommandBuffers[1];
    //	SubmitInfo.waitSemaphoreCount = 1;
    //	SubmitInfo.pWaitSemaphores = &Semaphore;
    //	SubmitInfo.pWaitDstStageMask = Flags;
    //	vkQueueSubmit(Queue, 1, &SubmitInfo, VK_NULL_HANDLE);
    //}

    ////auto ret = vkWaitForFences(Device, 1, &Fence, VK_TRUE, UINT64_MAX);

    //vkQueueWaitIdle(Queue);

    //vkDestroyCommandPool(Device, CommandPool, nullptr);
    //vkDestroyFence(Device, Fence, nullptr);
    //vkDestroySemaphore(Device, Semaphore, nullptr);

    return 0;
}