#include "Window.h"
#include "Resource/resource1.h"

namespace stay
{


	Window::WindowRegister Window::windowClass(L"WindowName");

	Window::WindowRegister::WindowRegister(LPCWSTR className)
	{
		windowClassName = className;
		hInstance = GetModuleHandle(LPCWSTR());

		WNDCLASSEX wcx = {};
		wcx.cbSize = sizeof(WNDCLASSEX);
		wcx.lpfnWndProc = WinProcBegin;
		wcx.cbClsExtra = 0;                // no extra class memory 
		wcx.cbWndExtra = 0;                // no extra window memory 
		wcx.hInstance = hInstance;         // handle to instance 
		wcx.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, GetSystemMetrics(SM_CYICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR));
		wcx.hCursor = nullptr;                 // predefined arrow 
		//wcx.hbrBackground = (HBRUSH)GetStockObject(
		//    WHITE_BRUSH);                  // white background brush 
		wcx.lpszMenuName = L"MainMenu";    // name of menu resource 
		wcx.lpszClassName = className;  // name of window class 
		wcx.hIconSm = (HICON)LoadImage(hInstance, // small class icon 
			MAKEINTRESOURCE(IMAGE_ICON),
			IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTCOLOR);

		RegisterClassEx(&wcx);

	}

	Window::WindowRegister::~WindowRegister()
	{
		UnregisterClassW(windowClassName, hInstance);
	}

	LPCWSTR Window::WindowRegister::GetWindowClassName() noexcept
	{
		return this->windowClassName;
	}

	HINSTANCE Window::WindowRegister::GetHInstance() noexcept
	{
		return this->hInstance;
	}

	Window::Window(LPCWSTR windowName, int x, int y)
	{
		DWORD dwExStyle = WS_EX_ACCEPTFILES | WS_EX_LEFT | WS_EX_NOINHERITLAYOUT | WS_EX_APPWINDOW;
		hWnd = CreateWindowEx(
			dwExStyle,
			windowClass.GetWindowClassName(),
			windowName,
			WS_OVERLAPPEDWINDOW,
			100, 100,
			x, y,
			nullptr,
			nullptr, nullptr, nullptr
		);

		if (hWnd != nullptr)
		{
			ShowWindow(hWnd, SW_SHOW);
		}
	}

	Window::~Window()
	{
		DestroyWindow(hWnd);
	}

	HWND Window::getHWnd() noexcept
	{
		return hWnd;
	}

	LRESULT Window::WinProcBegin(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
}
