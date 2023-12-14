#include "BaseApplication.h"

#include "Windows/RHI/GraphicsCore.h"

int stay::BaseApplication::initialize()
{
    m_bQuit = false;
    Graphics::Initialize();

    return 0;
}

void stay::BaseApplication::Tick()
{
    std::optional<int> result = Graphics::g_App->CheckMessage();
    if (result.has_value())
    {
        m_bQuit = true;
    }

}

void stay::BaseApplication::finalize()
{
    Graphics::Finalize();
}

bool stay::BaseApplication::isQuit() noexcept
{
    return false;
}
