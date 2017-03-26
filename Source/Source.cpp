#include "Rendering\Renderer.h"

int main()
{
	vulkan::Renderer R;

	R.OpenWindow(800, 600, "Vulkan Engine");

	while (R.Run())
	{

	}

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