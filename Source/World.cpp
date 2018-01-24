/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include "World.h"

#include "Core.h"

VULKAN_NS_USING;

VK_DECLARE_LOG_CATEGORY(LogWorld);

World::World(DevicePtr device)
    : VulkanClass (device)
{

}

World::~World()
{
    cameras.clear();
}

//void World::Destroy()
//{
//    cameras.clear();
//}

void World::Tick(float deltaTime)
{
    VK_PERFORMANCE_SECTION("World tick");
}

void World::AddCamera(Camera* camera)
{
    std::pair<std::set<Camera*>::iterator, bool> result = cameras.insert(camera);

    if (!result.second)
    {
        VK_LOG(LogWorld, Warning, "Camera already added, skipping...");
    }
}
