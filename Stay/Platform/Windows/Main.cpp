#include <iostream>

#include "WinStd.h"
#include "Window.h"
#include "WinError.h"
#include "WinHelper.h"




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
int WINAPI WinMain(HINSTANCE , HINSTANCE, LPSTR, int)
{
	stay::Window engineWindow(L"stay");
	try
	{

		throw stay::WindowException(__FILE__, __LINE__, 20);
	}
	catch (const stay::Exception &e)
	{
		
		MessageBoxA(
			nullptr, 
			e.what(), 
			e.getType(), 
			MB_CANCELTRYCONTINUE);


	}
	MSG msg;
	while (GetMessage(&msg, engineWindow.getHWnd(), 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}




