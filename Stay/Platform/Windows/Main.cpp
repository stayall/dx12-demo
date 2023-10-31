

#include "WinStd.h"
#include "Window.h"




LRESULT WinProc(
	HWND    hWnd,
	UINT    Msg,
	WPARAM  wParam,
	LPARAM  lParam
)
{
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}




_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	stay::Window engineWindow(L"stay");
	MSG msg;
	while (GetMessage(&msg, engineWindow.getHWnd(), 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}




