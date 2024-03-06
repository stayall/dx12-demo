#include <Platform/Windows/WinStd.h>
#include <Resource/SenceParse.h>
#include <Resource/AssetDef.h>


using namespace stay::Sence;

_Use_decl_annotations_
int WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    SenceParse aissimpSenceParse;
    auto cube = aissimpSenceParse.ParseSence(stay::Resource::g_AssetFilePath + "cube.fbx");

    return 0;
}