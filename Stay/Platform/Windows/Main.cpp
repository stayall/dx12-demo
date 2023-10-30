

#include "WinStd.h"



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
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)

{
	const wchar_t windowClassName[] = L"MainWindow";

	DWORD dwExStyle = WS_EX_ACCEPTFILES | WS_EX_LEFT | WS_EX_NOINHERITLAYOUT | WS_EX_APPWINDOW;
	WNDCLASSEX wcx = {};
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.lpfnWndProc = WinProc;
	wcx.cbClsExtra = 0;                // no extra class memory 
	wcx.cbWndExtra = 0;                // no extra window memory 
	wcx.hInstance = hInstance;         // handle to instance 
	//wcx.hIcon = LoadIcon(NULL,
	//    IDI_APPLICATION);              // predefined app. icon 
	//wcx.hCursor = LoadCursor(NULL,
	//    IDC_ARROW);                    // predefined arrow 
	//wcx.hbrBackground = (HBRUSH)GetStockObject(
	//    WHITE_BRUSH);                  // white background brush 
	wcx.lpszMenuName = L"MainMenu";    // name of menu resource 
	wcx.lpszClassName = windowClassName;  // name of window class 
	//wcx.hIconSm = (HICON)LoadImage(hInstance, // small class icon 
	//    MAKEINTRESOURCE(5),
	//    IMAGE_ICON,
	//    GetSystemMetrics(SM_CXSMICON),
	//    GetSystemMetrics(SM_CYSMICON),
	//    LR_DEFAULTCOLOR);

	RegisterClassEx(&wcx);

	HWND hWnd = CreateWindowEx(
		dwExStyle,
		windowClassName,
		L"Stay",
		WS_OVERLAPPEDWINDOW,
		500, 500,
		600, 500,
		nullptr,
		nullptr, nullptr, nullptr
	);

	if (hWnd != nullptr)
	{
		ShowWindow(hWnd, SW_SHOW);
	}
	MSG msg = {};
	while (GetMessage(&msg, hWnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}




