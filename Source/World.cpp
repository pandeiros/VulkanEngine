/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "World.h"

#include "Core.h"

VULKAN_NS_USING;

VK_DECLARE_LOG_CATEGORY(LogWorld);

void World::Update(float deltaTime)
{

}

void World::AddCamera(Camera* camera)
{
    std::pair<std::set<Camera*>::iterator, bool> result = cameras.insert(camera);

    if (!result.second)
    {
        VK_LOG(LogWorld, Warning, "Camera already added, skipping...");
    }
}
