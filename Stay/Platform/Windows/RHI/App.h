#ifndef _STAY_APPLICATION_H_
#define _STAY_APPLICATION_H_

#include <memory>
#include <optional>

#include "Window.h"
namespace stay
{
	class App
	{
	public:
		App();
		void Create(const WCHAR* name = L"stay");
		void ShutDown(){  }
		~App() = default;
		std::optional<int> CheckMessage();

		Window& GetWindow() const { return *window.get(); };
	private:
		MSG msg;
		std::unique_ptr<Window> window = nullptr;
	};
}
#endif // !_STAY_APPLICATION_H_

