#include "App.h"

namespace stay
{
	App::App()
	{
		window = std::make_unique<Window>(L"stay");
	}

	std::optional<int> App::CheckMessage()
	{
		PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		if (msg.message == WM_QUIT)
		{
			return { msg.wParam };
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		return {};
	}
}
