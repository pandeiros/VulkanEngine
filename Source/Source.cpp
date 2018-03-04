/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#include "Window.h"
#include "CommandPool.h"

#include <array>
#include <chrono>

#include "Application.h"
#include "Core.h"

VULKAN_NS_USING;

#ifndef __ANDROID__

#include "TestApplication.h"

int main()
{
    std::unique_ptr<TestApplication> testApplication(new TestApplication());

    testApplication->Init();

    while (1)
    {
        testApplication->GetEngine()->Update();
    }

    return 0;
}

#endif