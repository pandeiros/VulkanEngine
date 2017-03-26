#include "Platforms/Win32/Window_Win32.h"
//#include <vulkan/vulkan.h>
#include "Rendering/Window.h"
#include "Rendering/Renderer.h"
#include <cassert>
#include <string>

using namespace VULKAN_NS;

#if VK_USE_PLATFORM_WIN32_KHR

// MS-Windows event handling function:
LRESULT CALLBACK WindowsEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window * window = reinterpret_cast<Window*>(
		GetWindowLongPtrW(hWnd, GWLP_USERDATA));

	switch (uMsg)
	{
		case WM_CLOSE:
			window->Close();
			break;
		case WM_SIZE:
			// we get here if the window has changed size, we should rebuild most
			// of our window resources before rendering to this window again.
			// ( no need for this because our window sizing by hand is disabled )
			break;
		default:
			return (DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return 0;
}

uint64_t Window::Win32ClassIdCounter = 0;

void Window::CreateOSWindow()
{
	WNDCLASSEX win_class {};
	assert(SurfaceSizeX > 0);
	assert(SurfaceSizeY > 0);

	Win32Instance = GetModuleHandle(nullptr);
	Win32ClassName = WindowName + "_" + std::to_string(Win32ClassIdCounter);
	++Win32ClassIdCounter;

	// Initialize the window class structure:
	win_class.cbSize = sizeof(WNDCLASSEX);
	win_class.style = CS_HREDRAW | CS_VREDRAW;
	win_class.lpfnWndProc = WindowsEventHandler;
	win_class.cbClsExtra = 0;
	win_class.cbWndExtra = 0;
	win_class.hInstance = Win32Instance; // hInstance
	win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	win_class.lpszMenuName = NULL;
	win_class.lpszClassName = Win32ClassName.c_str();
	win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	// Register window class:
	if (!RegisterClassEx(&win_class))
	{
		// It didn't work, so try to give a useful error:
		assert(0 && "Cannot create a window in which to draw!\n");
		fflush(stdout);
		exit(-1);
	}

	DWORD ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX; // | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

																			// Create window with the registered class:
	RECT wr = {0, 0, LONG(SurfaceSizeX), LONG(SurfaceSizeY)};
	AdjustWindowRectEx(&wr, style, FALSE, ex_style);
	Win32Window = CreateWindowEx(0,
								 Win32ClassName.c_str(),		// class name
								 WindowName.c_str(),			// app name
								 style,							// window style
								 CW_USEDEFAULT, CW_USEDEFAULT,	// x/y coords
								 wr.right - wr.left,				// width
								 wr.bottom - wr.top,				// height
								 NULL,							// handle to parent
								 NULL,							// handle to menu
								 Win32Instance,				// hInstance
								 NULL);							// no extra parameters
	if (!Win32Window)
	{
		// It didn't work, so try to give a useful error:
		assert(1 && "Cannot create a window in which to draw!\n");
		fflush(stdout);
		exit(-1);
	}
	SetWindowLongPtr(Win32Window, GWLP_USERDATA, (LONG_PTR)this);

	ShowWindow(Win32Window, SW_SHOW);
	SetForegroundWindow(Win32Window);
	SetFocus(Win32Window);
}

void Window::DestroyOSWindow()
{
	DestroyWindow(Win32Window);
	UnregisterClass(Win32ClassName.c_str(), Win32Instance);
	Win32Window = nullptr;
}

void Window::UpdateOSWindow()
{
	MSG msg;
	if (PeekMessage(&msg, Win32Window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Window::CreateOSSurface()
{
	VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo {};
	SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	SurfaceCreateInfo.hinstance = Win32Instance;
	SurfaceCreateInfo.hwnd = Win32Window;
	ErrorCheck(vkCreateWin32SurfaceKHR(MainRenderer->GetInstance(), &SurfaceCreateInfo, nullptr, &Surface));
}

#endif