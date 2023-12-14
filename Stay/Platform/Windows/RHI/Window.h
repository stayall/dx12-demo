#ifndef _STAY_WINDOW_H_
#define _STAY_WINDOW_H_

#include <optional>

#include "../WinStd.h"

namespace stay
{

	class Window
	{
	public:
		Window(LPCWSTR windowName, int x = 1000, int y = 600);
		~Window();

		HWND getHWnd() const noexcept;
		UINT getWidth() const noexcept { return width; }
		UINT getHeight() const noexcept { return height; }

		class WindowRegister
		{
		public:
			WindowRegister(LPCWSTR windowClassName);
			~WindowRegister();

			LPCWSTR GetWindowClassName() noexcept;
			HINSTANCE GetHInstance() noexcept;
		private:
			HINSTANCE hInstance;
			LPCWSTR windowClassName;
		};

	private:
		static WindowRegister& GetWindowRegister() ;
		static LRESULT WinProcBegin(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
		static LRESULT WinProcHandle(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
		LRESULT WinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	private:
		UINT width;
		UINT height;
		HWND m_hWnd;
	};
}

#endif // !_STAY_WINDOW_H_

