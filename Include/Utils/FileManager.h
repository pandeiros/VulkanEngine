/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#pragma once

#include "VulkanCore.h"

#ifdef __ANDROID__
#include "AndroidUtils.h"
#endif

#include "Rendering/RenderComponent.h"

/**
 * @file FileManager.h
 */

VULKAN_NS_BEGIN

/**
 * @class FileManager
 */
class FileManager
{
public:
    static bool LoadOBJ(const std::string filename, VertexData& vertices, std::vector<uint32_t>& indices);
    static bool LoadOBJWithNormals(const std::string filename, VertexData& vertices, std::vector<uint32_t>& indices, std::vector<glm::vec3>& normals);
    static std::string GetAssetDirectory();
};

VULKAN_NS_END