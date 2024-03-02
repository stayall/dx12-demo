#include "../SenceNode.h"
#include "../SenceObject.h"
#include "../Sence.h"

#include "Platform/Windows/WinStd.h"

using namespace stay::Sence;

_Use_decl_annotations_
int WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    Sence sence;
    auto camera = std::make_shared<Camera>();
    auto cameraNode = std::make_shared<CameraNode>(camera);
    auto light = std::make_shared<Light>();
    auto lightNode = std::make_shared<LightNode>(light);
    auto geometry = std::make_shared<Geometry>();
    auto geometryNode = std::make_shared<GeometryNode>(geometry);


    sence.m_senceNode->AddChild(cameraNode);

}
