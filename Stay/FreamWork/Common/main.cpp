#include <iostream>


#include "../Common/BaseApplication.h"

namespace stay
{
	extern IApplication* g_pApp;
}


int main(int argc, char** argv)
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
	
}