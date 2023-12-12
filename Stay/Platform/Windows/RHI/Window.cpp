#include "Window.h"
#include "WinError.h"
#include "Resource/resource1.h"

namespace stay
{



	Window::WindowRegister::WindowRegister(LPCWSTR className)
	{
		windowClassName = className;
		hInstance = GetModuleHandle(LPCWSTR());
		if (hInstance == nullptr)
		{
			THROW_LASTEXCEPTION();
		}

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

		if (RegisterClassEx(&wcx) == 0)
		{
			THROW_LASTEXCEPTION();
		}

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
		width = x;
		height = y;

		DWORD dwExStyle = WS_EX_ACCEPTFILES | WS_EX_LEFT | WS_EX_NOINHERITLAYOUT | WS_EX_APPWINDOW;
		m_hWnd = CreateWindowEx(
			dwExStyle,
			GetWindowRegister().GetWindowClassName(),
			windowName,
			WS_OVERLAPPEDWINDOW,
			100, 100,
			width, height,
			nullptr,
			nullptr,
			GetModuleHandle(LPCWSTR()),
			reinterpret_cast<LPVOID>(this)
		);

		if (m_hWnd == nullptr)
		{
			THROW_LASTEXCEPTION();
		}

		ShowWindow(m_hWnd, SW_SHOW);
	}

	Window::~Window()
	{
		DestroyWindow(m_hWnd);
	}

	HWND Window::getHWnd() const noexcept
	{
		return m_hWnd;
	}


	Window::WindowRegister& Window::GetWindowRegister()
	{
	    static WindowRegister windowClass(L"WindowName");
		return windowClass;
	}

	LRESULT Window::WinProcBegin(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		CREATESTRUCT* pCreate;
		Window* pWindow;
		switch (Msg)
		{
		case WM_CREATE:
		case WM_NCCREATE:
			pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WinProcHandle));
		default:
			break;
		}
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	LRESULT Window::WinProcHandle(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		auto pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		return pWindow->WinProc(hWnd, Msg, wParam, lParam);
	}

	LRESULT Window::WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		switch (Msg)
		{
		case WM_CLOSE:
			if (MessageBox(hWnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
			{
				PostQuitMessage(0);
			}
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			break;
		}
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
}
