#include "RootSignature.h"

#include "WinError.h"
#include "GraphicsCore.h"

namespace stay
{
	D3D12_ROOT_SIGNATURE_FLAGS RootSignature::s_FullAcceseFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	void RootSignatureParameter::AsDescriptorTable(UINT startRange, UINT endRange, D3D12_SHADER_VISIBILITY shaderVisibilty)
	{
		ASSERT(startRange >= 0 && endRange < m_ranges.size(), "Deecriptor Range Index Out Vector Index Range");

		D3D12_ROOT_DESCRIPTOR_TABLE1 table{};
		table.NumDescriptorRanges = endRange - startRange + 1;
		table.pDescriptorRanges = &m_ranges[startRange];
		m_parameter.DescriptorTable = std::move(table);
		m_parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		m_parameter.ShaderVisibility = shaderVisibilty;
	}

	void RootSignatureParameter::AddDescriptorRange(D3D12_DESCRIPTOR_RANGE1* ranges, UINT numRanges)
	{

		for (UINT i = 0; i < numRanges; i++)
		{
			m_ranges.push_back(ranges[i]);
		}
	}
	void RootSignatureParameter::SetDescriptorRangeSize(UINT numRanges)
	{
		m_ranges.resize(numRanges);
	}

	void RootSignatureParameter::SetDescriptorRange(UINT rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, UINT baseShaderRegister, UINT registerSpace, UINT numDescriptors, D3D12_DESCRIPTOR_RANGE_FLAGS flags, UINT offsetInDescriptorsFromTableStart)
	{
		ASSERT(rangeIndex <= m_ranges.size());
		auto& range = m_ranges[rangeIndex];
		range.RangeType = rangeType;
		range.NumDescriptors = numDescriptors;
		range.BaseShaderRegister = baseShaderRegister;
		range.RegisterSpace = registerSpace;
		range.Flags = flags;
		range.OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
	}

	RootSignature::RootSignature(const wchar_t* name, D3D12_ROOT_SIGNATURE_FLAGS acceseFlag):
		m_name(name)
	{
		ZeroMemory(&m_State, sizeof(m_State));
		m_State.Flags = acceseFlag;
		m_rootSignature = nullptr;
		m_parameters.clear();
		m_rootParameters.reset(nullptr);
	}

	D3D12_FEATURE_DATA_ROOT_SIGNATURE RootSignature::CheckFeatureSupport()
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE rootFeature{};
		rootFeature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (Graphics::g_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootFeature, sizeof(rootFeature)) == E_INVALIDARG)
		{
			rootFeature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		return rootFeature;
	}

	void RootSignature::SetParameters(UINT numParameters, RootSignatureParameter* parameters)
	{
		m_parameters.resize(numParameters);
		for (UINT i = 0; i < numParameters; i++)
		{
			m_parameters[i] = parameters[i];
		}
	}

	void RootSignature::SetParamerter(UINT indexParater, RootSignatureParameter paraters)
	{
		ASSERT(indexParater < m_parameters.size());
		m_parameters[indexParater] = paraters;
	}

	void RootSignature::Finalize(ID3D12Device* device)
	{
		ASSERT(m_rootSignature == nullptr);
		m_State.NumParameters = (UINT)m_parameters.size();

		m_rootParameters.reset(new D3D12_ROOT_PARAMETER1[m_State.NumParameters]);
		auto arrayStart = m_rootParameters.get();
		for (size_t i = 0; i < m_State.NumParameters; i++)
		{
			*(arrayStart + i) = m_parameters[i].m_parameter;
		}
		m_State.pParameters = arrayStart;

		D3D12_FEATURE_DATA_ROOT_SIGNATURE rootFeature = CheckFeatureSupport();

		ASSERT(rootFeature.HighestVersion == D3D_ROOT_SIGNATURE_VERSION_1_1);
		D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionRoot{};
		versionRoot.Version = rootFeature.HighestVersion;
		versionRoot.Desc_1_1 = m_State;

		using Microsoft::WRL::ComPtr;

		ComPtr<ID3DBlob> signatrue;
		ComPtr<ID3DBlob> error;
		THROW_IF_FAILED(D3D12SerializeVersionedRootSignature(&versionRoot, signatrue.GetAddressOf(), error.GetAddressOf()));
		THROW_IF_FAILED(device->GetDeviceRemovedReason());
		THROW_IF_FAILED(device->CreateRootSignature(0, signatrue->GetBufferPointer(), signatrue->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
		m_rootSignature->SetName(m_name);
	}
}
