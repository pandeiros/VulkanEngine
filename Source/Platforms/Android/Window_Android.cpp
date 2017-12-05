#include "Platforms/Win32/Window_Win32.h"
#include "Rendering/Windows_old.h"  // #TODO Delete
#include "Window.h"
#include "Instance.h"
#include "Rendering/Renderer.h"
#include <cassert>
#include <string>

VULKAN_NS_USING;

#ifdef __ANDROID__

void Window::CreateOSWindow()
{
}

void Window::CreateOSSurface()
{
    // #TODO FIX THIS
    //GET_INSTANCE_PROC_ADDR(cachedInstance->GetVkInstance(), CreateAndroidSurfaceKHR);

    //VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo;
    //surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    //surfaceCreateInfo.pNext = nullptr;
    //surfaceCreateInfo.flags = 0;
    //surfaceCreateInfo.window = AndroidGetApplicationWindow();
    ////res = fpCreateAndroidSurfaceKHR(info.inst, &surfaceCreateInfo, nullptr, &info.surface);

    //DebugTools::Verify(fpCreateAndroidSurfaceKHR(info.inst, &surfaceCreateInfo, nullptr, &info.surface));
}

void Window::UpdateOSWindow()
{
    //MSG msg;
    //if (PeekMessage(&msg, win32Window, 0, 0, PM_REMOVE))
    //{
    //    TranslateMessage(&msg);
    //    DispatchMessage(&msg);
    //}
}

void Window::DestroyOSWindow()
{
    //DestroyWindow(win32Window);
    //UnregisterClass(win32ClassName.c_str(), win32Instance);
    //win32Window = nullptr;
}

#endif // ANDROID