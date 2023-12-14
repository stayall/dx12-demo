#include "App.h"

namespace stay
{

	App::App()
	{
		window = std::make_unique<Window>(L"name");
	}

	void App::Create(const WCHAR* name)
	{
		window = std::make_unique<Window>(name);
	}

	std::optional<int> App::CheckMessage()
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				return { (int)msg.wParam };
			}
		}

		return {};
	}
}
