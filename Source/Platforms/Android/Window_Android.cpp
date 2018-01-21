#include "Window.h"
#include "Core.h"
#include "Instance.h"
//#include "Rendering/Renderer.h"
#include "AndroidUtils.h"

#include <cassert>
#include <string>

VULKAN_NS_USING;

#ifdef __ANDROID__

void Window::CreateOSWindow()
{
}

void Window::CreateOSSurface()
{
    VULKAN_GET_INSTANCE_PROC_ADDR(instance, CreateAndroidSurfaceKHR);

    VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {
        VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
        nullptr,
        0,
        AndroidGetApplicationWindow()
    };

    VK_VERIFY(vkCreateAndroidSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));
}

void Window::UpdateOSWindow()
{}

void Window::DestroyOSWindow()
{}

#endif // __ANDROID__