#include "VulkanConfig.h"

VULKAN_NS_USING;

void ErrorCheck(VkResult Result)
{
#if VULKAN_ENABLE_RUNTIME_DEBUG
	if (Result < 0)
	{
		switch (Result)
		{
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				std::cout << "VK_ERROR_OUT_OF_HOST_MEMORY\n";
				break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				std::cout << "VK_ERROR_OUT_OF_DEVICE_MEMORY\n";
				break;
			case VK_ERROR_INITIALIZATION_FAILED:
				std::cout << "VK_ERROR_INITIALIZATION_FAILED\n";
				break;
			case VK_ERROR_DEVICE_LOST:
				std::cout << "VK_ERROR_DEVICE_LOST\n";
				break;
			case VK_ERROR_MEMORY_MAP_FAILED:
				std::cout << "VK_ERROR_MEMORY_MAP_FAILED\n";
				break;
			case VK_ERROR_LAYER_NOT_PRESENT:
				std::cout << "VK_ERROR_LAYER_NOT_PRESENT\n";
				break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				std::cout << "VK_ERROR_EXTENSION_NOT_PRESENT\n";
				break;
			case VK_ERROR_FEATURE_NOT_PRESENT:
				std::cout << "VK_ERROR_FEATURE_NOT_PRESENT\n";
				break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				std::cout << "VK_ERROR_INCOMPATIBLE_DRIVER\n";
				break;
			case VK_ERROR_TOO_MANY_OBJECTS:
				std::cout << "VK_ERROR_TOO_MANY_OBJECTS\n";
				break;
			case VK_ERROR_FORMAT_NOT_SUPPORTED:
				std::cout << "VK_ERROR_FORMAT_NOT_SUPPORTED\n";
				break;
			case VK_ERROR_FRAGMENTED_POOL:
				std::cout << "VK_ERROR_FRAGMENTED_POOL\n";
				break;
			case VK_ERROR_SURFACE_LOST_KHR:
				std::cout << "VK_ERROR_SURFACE_LOST_KHR\n";
				break;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				std::cout << "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR\n";
				break;
			case VK_SUBOPTIMAL_KHR:
				std::cout << "VK_SUBOPTIMAL_KHR\n";
				break;
			case VK_ERROR_OUT_OF_DATE_KHR:
				std::cout << "VK_ERROR_OUT_OF_DATE_KHR\n";
				break;
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
				std::cout << "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR\n";
				break;
			case VK_ERROR_VALIDATION_FAILED_EXT:
				std::cout << "VK_ERROR_VALIDATION_FAILED_EXT\n";
				break;
			case VK_ERROR_INVALID_SHADER_NV:
				std::cout << "VK_ERROR_INVALID_SHADER_NV\n";
				break;
			default:
				break;
		}

		//assert(0 && "Vulkan runtime error.");
	}

#endif // VULKAN_ENABLE_RUNTIME_DEBUG
}

uint32_t GetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties * PhysicalDeviceMemoryProperties, const VkMemoryRequirements * MemoryRequirements, const VkMemoryPropertyFlags RequiredProperties)
{
	for (uint32_t i = 0; i < PhysicalDeviceMemoryProperties->memoryTypeCount; ++i)
	{
		if (MemoryRequirements->memoryTypeBits & (1 << i))
		{
			if ((PhysicalDeviceMemoryProperties->memoryTypes[i].propertyFlags & RequiredProperties) == RequiredProperties)
			{
				return i;
			}
		}
	}

	//assert(0 && "Could not find property memory type.");

	return UINT32_MAX;
}
