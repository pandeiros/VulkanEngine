#include "Rendering/Renderer.h"
#include <cstdlib>
#include <assert.h>
#include <iostream>
#include <sstream>

using namespace VULKAN_NS;

#ifdef _WIN32
#include "Windows.h"
#endif

Renderer::Renderer()
{
	SetupDebug();
	InitInstance();
	InitDebug();
	InitDevice();
}

Renderer::~Renderer()
{
	DestroyDevice();
	DestroyDebug();
	DestroyInstance();
}

void Renderer::InitInstance()
{
	VkApplicationInfo ApplicationInfo {};
	ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ApplicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 30);
	ApplicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	ApplicationInfo.pApplicationName = "Vulkan Engine";

	VkInstanceCreateInfo InstanceCreateInfo {};
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
	InstanceCreateInfo.enabledLayerCount = InstanceLayers.size();
	InstanceCreateInfo.ppEnabledLayerNames = InstanceLayers.data();
	InstanceCreateInfo.enabledExtensionCount = InstaceExtensions.size();
	InstanceCreateInfo.ppEnabledExtensionNames = InstaceExtensions.data();
	InstanceCreateInfo.pNext = &DebugCallbackCreateInfo;

	auto Error = vkCreateInstance(&InstanceCreateInfo, nullptr, &Instance);
	if (Error != VK_SUCCESS)
	{
		assert(0 && "Vulkan ERROR: Create instance failed.");
		std::exit(-1);
	}
}

void Renderer::DestroyInstance()
{
	vkDestroyInstance(Instance, nullptr);
	Instance = nullptr;
}

void Renderer::InitDevice()
{
	uint32_t GPUCount = 0;
	vkEnumeratePhysicalDevices(Instance, &GPUCount, nullptr);
	std::vector<VkPhysicalDevice> GPUList(GPUCount);
	vkEnumeratePhysicalDevices(Instance, &GPUCount, GPUList.data());
	GPU = GPUList[0];

	vkGetPhysicalDeviceProperties(GPU, &GPUProperties);

	uint32_t FamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(GPU, &FamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> FamilyPropertyList(FamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(GPU, &FamilyCount, FamilyPropertyList.data());

	bool FamilyIndexFound = false;
	for (uint32_t i = 0; i < FamilyCount; ++i)
	{
		if (FamilyPropertyList[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			GraphicsFamilyIndex = 1;
			FamilyIndexFound = true;
		}
	}

	if (!FamilyIndexFound)
	{
		assert(0 && "Vulkan ERROR: Queue family supporting graphics not found.");
		std::exit(-1);
	}

	uint32_t LayerCount = 0;
	vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
	std::vector<VkLayerProperties> LayerPropertyList(LayerCount);
	vkEnumerateInstanceLayerProperties(&LayerCount, LayerPropertyList.data());

	std::cout << "Instance layers:\n";
	for (auto &Property : LayerPropertyList)
	{
		std::cout << "  " << Property.layerName << "\t\t | " << Property.description << "\n";
	}

	float PriorityQueue[] {1.f};
	VkDeviceQueueCreateInfo DeviceQueueCreateInfo {};
	DeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	DeviceQueueCreateInfo.queueFamilyIndex = GraphicsFamilyIndex;
	DeviceQueueCreateInfo.queueCount = 1;
	DeviceQueueCreateInfo.pQueuePriorities = PriorityQueue;

	VkDeviceCreateInfo DeviceCreateInfo {};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.queueCreateInfoCount = 1;
	DeviceCreateInfo.pQueueCreateInfos = &DeviceQueueCreateInfo;

	auto Error = vkCreateDevice(GPU, &DeviceCreateInfo, nullptr, &Device);
	if (Error != VK_SUCCESS)
	{
		assert(0 && "Vulkan ERROR: Device creation failed.");
		std::exit(-1);
	}
}

void Renderer::DestroyDevice()
{
	vkDestroyDevice(Device, nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugCallback(VkDebugReportFlagsEXT MsgFlags, VkDebugReportObjectTypeEXT ObjectType, uint64_t SourceObject,
					size_t Location, int32_t MsgCode, const char* LayerPrefix, const char* Msg, void* UserData)
{
	std::ostringstream oss;
	if (MsgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
	{
		oss << "INFO: ";
	}
	if (MsgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		oss << "WARNING: ";
	}
	if (MsgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
	{
		oss << "PERFORMANCE: ";
	}
	if (MsgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		oss << "ERROR: ";
	}
	if (MsgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
	{
		oss << "DEBUG: ";
	}

	oss << "@[" << LayerPrefix << "] " << Msg << "\n";
	std::cout << oss.str();

#ifdef _WIN32
	if (MsgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		MessageBox(NULL, oss.str().c_str(), "Vulkan Error!", 0);
#endif

	return false;
}

void Renderer::SetupDebug()
{
	DebugCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	DebugCallbackCreateInfo.pfnCallback = VulkanDebugCallback;
	DebugCallbackCreateInfo.flags =
		VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
		VK_DEBUG_REPORT_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_ERROR_BIT_EXT |
		VK_DEBUG_REPORT_DEBUG_BIT_EXT |
		0;

	InstanceLayers.push_back("VK_LAYER_GOOGLE_threading");
	InstanceLayers.push_back("VK_LAYER_LUNARG_image");
	InstanceLayers.push_back("VK_LAYER_LUNARG_object_tracker");
	InstanceLayers.push_back("VK_LAYER_LUNARG_parameter_validation");

	InstaceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
}

PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = nullptr;
PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = nullptr;

void Renderer::InitDebug()
{
	fvkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugReportCallbackEXT");
	fvkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugReportCallbackEXT");

	if (!fvkCreateDebugReportCallbackEXT || !fvkDestroyDebugReportCallbackEXT)
	{
		assert(0 && "Vulkan ERROR: Cannot fetch debug function pointers.");
		std::exit(-1);
	}

	fvkCreateDebugReportCallbackEXT(Instance, &DebugCallbackCreateInfo, nullptr, &DebugReport);
}

void Renderer::DestroyDebug()
{
	fvkDestroyDebugReportCallbackEXT(Instance, DebugReport, nullptr);
	DebugReport = nullptr;
}
