/*
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include "Platform.h"
//#include <vulkan/vulkan.h>
#include <iostream>
#include <cassert>
#include <vector>
#include "BuildOptions.h"


/*
 * @file VulkanConfig.h
 */

#define VULKAN_NS vulkan

namespace VULKAN_NS
{
	void ErrorCheck(VkResult Result);

	uint32_t GetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties * PhysicalDeviceMemoryProperties, const VkMemoryRequirements * MemoryRequirements, const VkMemoryPropertyFlags MemoryProperties);

	template<typename T>
	bool IsOfEnum(T Value, std::vector<T> Enums)
	{
		for (auto & Enum : Enums)
		{
			if (Value == Enum)
				return true;
		}

		return false;
	}
}