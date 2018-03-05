/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#include "Utils/FileManager.h"

#include "Core.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

VK_DECLARE_LOG_CATEGORY(LogFileManager);

VULKAN_NS_USING;

bool FileManager::LoadOBJ(const std::string filename, VertexData& vertices, std::vector<uint32_t>& indices)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

#ifdef __ANDROID__
    std::vector<char> buffer = AndroidUtils::GetFileContent(GetAssetDirectory() + filename);
    wrap_vector_as_istream databuf(buffer);
    std::istream is(&databuf);
    if (tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &is))
#else
    if (tinyobj::LoadObj(&attrib, &shapes, &materials, &err, (GetAssetDirectory() + filename).c_str()))
#endif
    {
        for (const auto &shape : shapes)
        {
            for (const auto &indice : shape.mesh.indices)
            {
                Vertex vertex = {};

                vertex.x = attrib.vertices[3 * indice.vertex_index + 0];
                vertex.y = attrib.vertices[3 * indice.vertex_index + 1];
                vertex.z = attrib.vertices[3 * indice.vertex_index + 2];
                vertex.w = 1.f;

                vertex.r = vertex.g = vertex.b = 0.8f;
                vertex.a = 1.f;

                vertices.push_back(vertex);
                indices.push_back((uint32_t)indices.size());
            }
        }

        VK_LOG(LogFileManager, Debug, "Loaded model has %d vertices.", (uint32_t)vertices.size());

        return true;
    }
    else
    {
        VK_LOG(LogFileManager, Error, "Cannot load OBJ file!\n%s", err.c_str());
    }

    return false;
}

std::string FileManager::GetAssetDirectory()
{
#ifdef __ANDROID__
    return "";
#else
    return "data/";
#endif

    VK_PLATFORM_NOT_SUPPORTED;

    return "";
}
