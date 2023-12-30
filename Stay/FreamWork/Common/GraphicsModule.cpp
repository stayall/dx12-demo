#include "GraphicsModule.h"

#include "Platform/Windows/RHI/Display.h"

namespace stay::GameCore
{

	int GraphicsModule::initialize()
	{
		Graphics::Initialize();
		Display::Initialize();

		return 0;
	}

	void GraphicsModule::finalize()
	{
		Display::ShutDown();
		Graphics::Finalize();
	}

	void GraphicsModule::Tick()
	{
	}
}
