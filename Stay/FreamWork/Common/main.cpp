#include <iostream>

#include "Windows/RHI/WinStd.h"
#include "../Common/BaseApplication.h"

namespace stay
{
	extern IApplication* g_pApp;
}


_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	if (stay::g_pApp->initialize() != 0)
	{
		return -1;
	}

	while (!stay::g_pApp->isQuit())
	{
		std::cout << "Tick" << std::endl;
		stay::g_pApp->Tick();
	}
	
	return 0;
}