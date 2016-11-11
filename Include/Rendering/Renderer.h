/*
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanConfig.h"

/*
 * @file Renderer.h
 */

namespace VULKAN_NS
{
	/*
	 * @class Renderer
	 */
	class Renderer
	{
	public:
		/**
		 * Default constructor.
		 */
		Renderer();

		/**
		 * Default destructor.
		 */
		~Renderer();

	//private:
		void InitInstance();

		void DestroyInstance();

		void InitDevice();

		void DestroyDevice();

		void SetupDebug();

		void InitDebug();

		void DestroyDebug();

		VkInstance Instance = VK_NULL_HANDLE;
		VkPhysicalDevice GPU = VK_NULL_HANDLE;
		VkDevice Device = VK_NULL_HANDLE;
		VkQueue Queue = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties GPUProperties = {};

		uint32_t GraphicsFamilyIndex = 0;

		std::vector<const char*> InstanceLayers;
		std::vector<const char*> InstaceExtensions;

		VkDebugReportCallbackEXT DebugReport = VK_NULL_HANDLE;
		VkDebugReportCallbackCreateInfoEXT DebugCallbackCreateInfo = {};
	};
}