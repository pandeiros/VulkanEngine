#include "Platforms/Win32/Window_Win32.h"
#include "Rendering/Windows_old.h"  // #TODO Delete
#include "Window.h"
#include "Instance.h"
#include "Rendering/Renderer.h"
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
            // We get here if the window has changed size, we should rebuild most
            // of our window resources before rendering to this window again
            // (no need for this if our window sizing by hand is disabled ).
            break;
        default:
            return (DefWindowProc(hWnd, uMsg, wParam, lParam));
    }
    return 0;
}

uint64_t Window::win32ClassIdCounter = 0;

void Window::CreateOSWindow()
{
    PhysicalDevice* physicalDevice = cachedInstance->GetDeviceRef().GetPhysicalDevice();
    VK_ASSERT(vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice->GetVkPhysicalDevice(), physicalDevice->GetGraphicsQueueFamilyIndex()), "Queue family does not support presentation.");

    WNDCLASSEX win_class {};

    DebugTools::Assert(windowCreateInfo.surfaceSize.width > 0, "Surface width is equal to or less than 0!");
    DebugTools::Assert(windowCreateInfo.surfaceSize.height > 0, "Surface height is equal to or less than 0!");

    win32Instance = GetModuleHandle(nullptr);
    win32ClassName = windowCreateInfo.windowName + "_" + std::to_string(win32ClassIdCounter);
    ++win32ClassIdCounter;

    // Initialize the window class structure:
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = WindowsEventHandler;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = win32Instance; // hInstance
    win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    win_class.lpszMenuName = NULL;
    win_class.lpszClassName = win32ClassName.c_str();
    win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    // Register window class:
    if (!RegisterClassEx(&win_class))
    {
        // #TODO assert with DebugTools
        // It didn't work, so try to give a useful error:
        assert(0 && "Cannot create a window in which to draw!\n");
        fflush(stdout);
        exit(-1);
    }

    DWORD ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX; // | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    // Create window with the registered class:
    RECT wr = {0, 0, LONG(windowCreateInfo.surfaceSize.width), LONG(windowCreateInfo.surfaceSize.height)};
    AdjustWindowRectEx(&wr, style, FALSE, ex_style);
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
        NULL);							        // no extra parameters

    if (!win32Window)
    {
        // #TODO assert with DebugTools
        // It didn't work, so try to give a useful error:
        assert(1 && "Cannot create a window in which to draw!\n");
        fflush(stdout);
        exit(-1);
    }

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

    DebugTools::Verify(vkCreateWin32SurfaceKHR(cachedInstance->GetVkInstance(), &surfaceCreateInfo, nullptr, &surface));
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