/**
 * Vulkan Engine
 *
 * Copyright (C) 2016-2018 Pawel Kaczynski
 */

#include "Platforms/Win32/Window_Win32.h"
#include "Window.h"
#include "Instance.h"
#include "Rendering/Renderer.h"
#include "Core.h"

#include <cassert>
#include <string>

VULKAN_NS_USING;

#if _WIN32

// MS Windows event handling function:
LRESULT CALLBACK WindowsEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Window* window = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

    switch (uMsg)
    {
        case WM_CLOSE:
            window->Close();
            break;
        case WM_SIZE:
            break;
        default:
            return (DefWindowProc(hWnd, uMsg, wParam, lParam));
    }
    return 0;
}

uint64_t Window::win32ClassIdCounter = 0;

void Window::CreateOSWindow()
{
    PhysicalDevice* physicalDevice = device->GetPhysicalDevice();
    VK_ASSERT(vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice->GetVkPhysicalDevice(),
        physicalDevice->GetGraphicsQueueFamilyIndex()), "Queue family does not support presentation.");

    WNDCLASSEX winClass {};

    VK_ASSERT(windowCreateInfo.surfaceSize.width > 0, "Surface width is equal to 0!");
    VK_ASSERT(windowCreateInfo.surfaceSize.height > 0, "Surface height is equal to 0!");

    win32Instance = GetModuleHandle(nullptr);
    win32ClassName = windowCreateInfo.windowName + "_" + std::to_string(win32ClassIdCounter);
    ++win32ClassIdCounter;

    // Initialize the window class structure.
    winClass.cbSize = sizeof(WNDCLASSEX);
    winClass.style = CS_HREDRAW | CS_VREDRAW;
    winClass.lpfnWndProc = WindowsEventHandler;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;
    winClass.hInstance = win32Instance;
    winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    winClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    winClass.lpszMenuName = NULL;
    winClass.lpszClassName = win32ClassName.c_str();
    winClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    // Register window class.
    VK_ASSERT(RegisterClassEx(&winClass), "Cannot register window class.");

    DWORD exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX; // | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    // Create window with the registered class:
    RECT wr = {0, 0, LONG(windowCreateInfo.surfaceSize.width), LONG(windowCreateInfo.surfaceSize.height)};
    AdjustWindowRectEx(&wr, style, FALSE, exStyle);
    win32Window = CreateWindowEx(0,
        win32ClassName.c_str(),		            // class name
        windowCreateInfo.windowName.c_str(),    // app name
        style,							        // window style
        CW_USEDEFAULT, CW_USEDEFAULT,	        // x/y coordinates
        wr.right - wr.left,				        // width
        wr.bottom - wr.top,				        // height
        NULL,							        // handle to parent
        NULL,							        // handle to menu
        win32Instance,				            // hInstance
        NULL                                    // no extra parameters
    );

    VK_ASSERT(win32Window, "Cannot create window.");

    SetWindowLongPtr(win32Window, GWLP_USERDATA, (LONG_PTR)this);

    ShowWindow(win32Window, SW_SHOW);
    SetForegroundWindow(win32Window);
    SetFocus(win32Window);
}

void Window::CreateOSSurface()
{
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
        VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        nullptr,
        0,
        win32Instance,
        win32Window
    };

    VK_VERIFY(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));
}

void Window::UpdateOSWindow()
{
    MSG msg;
    if (PeekMessage(&msg, win32Window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Window::DestroyOSWindow()
{
    DestroyWindow(win32Window);
    UnregisterClass(win32ClassName.c_str(), win32Instance);
    win32Window = nullptr;
}

#endif