#include <optional>

#include "BaseApplication.h"

#include "GraphicsModule.h"
#include "Platform/Windows/RHI/GraphicsCore.h"


namespace stay
{

	int BaseApplication::initialize()
	{
		m_bQuit = false;
		auto graphicsModule = std::make_unique<GameCore::GraphicsModule>();
		graphicsModule->initialize();
		m_modules.emplace_back(std::move(graphicsModule));

		return 0;
	}

	void BaseApplication::Tick()
	{
		std::optional<int> result = Graphics::g_App->CheckMessage();
		if (result.has_value())
		{
			m_bQuit = true;
		}

		for (auto& module : m_modules)
		{
			module->Tick();
		}
		
	}

	void BaseApplication::finalize()
	{

		for (auto module = m_modules.rbegin(); module != m_modules.rend(); ++module)
		{
			(*module)->finalize();
		}
	}

	bool BaseApplication::isQuit() noexcept
	{
		return false;
	}
}