#pragma once

#include <DirectXMath.h>

namespace stay
{
	enum VertexDataType
	{
		kPosition,
		kNormal,
		kTangent,
		kColor,
		kUV,
		kNumVertexDataType
	};

	struct VertexData
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 normal;
		DirectX::XMFLOAT4 tangent;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 UV;
	};
}