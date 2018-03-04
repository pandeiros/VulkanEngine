/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2017 Pawel Kaczynski
 */

#include <AndroidUtils.h>
#include <TestApplication.h>

int vulkan_android_main(int argc, char *argv[])
{
    if (!vulkan::AndroidUtils::nativeApplication)
    {
        return VK_NOT_READY;
    }

    vulkan::AndroidUtils::vulkanApplication.reset(new vulkan::TestApplication());

    return 0;
}
