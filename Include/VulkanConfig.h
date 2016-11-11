/*
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#pragma once

#include <vulkan/vulkan.h>
#include <iostream>
#include <cassert>

/*
 * @file VulkanConfig.h
 */

#define VULKAN_NS vulkan

namespace VULKAN_NS
{
	void ErrorCheck(VkResult Result);
}