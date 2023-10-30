#include "BaseApplication.h"

int stay::BaseApplication::initialize()
{
    m_bQuit = false;
    
    return 0;
}

void stay::BaseApplication::Tick()
{
}

void stay::BaseApplication::finalize()
{
}

bool stay::BaseApplication::isQuit() noexcept
{
    return false;
}
