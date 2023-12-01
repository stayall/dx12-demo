#pragma once
#include <memory>
#include <vector>

#include "WinStd.h"

namespace stay
{
	class RootSignature;

	class RootSignatureParameter
	{
		friend RootSignature;
	public:
		void AsDescriptorTable(UINT startRange, UINT endRange, D3D12_SHADER_VISIBILITY shaderVisibilty = D3D12_SHADER_VISIBILITY_ALL);
		void AddDescriptorRange(D3D12_DESCRIPTOR_RANGE1* ranges, UINT numRanges);
		void SetDescriptorRangeSize(UINT numRanges);
		void SetDescriptorRange(
			UINT rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
			UINT baseShaderRegister, UINT registerSpace,
			UINT numDescriptors = 1, D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,
			UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);


	private:
		D3D12_ROOT_PARAMETER1 m_parameter{};
		std::vector<D3D12_DESCRIPTOR_RANGE1> m_ranges;
	};

	class RootSignature
	{
	public:
		RootSignature(const wchar_t* name = L"Unamed Root Signature", D3D12_ROOT_SIGNATURE_FLAGS acceseFlag = s_FullAcceseFlags);
		static D3D12_FEATURE_DATA_ROOT_SIGNATURE CheckFeatureSupport();

		void SetParametersSize(UINT numParaneters) { m_parameters.resize(numParaneters); }
		void SetParameters(UINT numParaneters, RootSignatureParameter *parameters);
		void SetParamerter(UINT indexParameter, RootSignatureParameter parameters);
		void AddParameter(RootSignatureParameter parameter) { m_parameters.push_back(parameter); }

		RootSignatureParameter& operator[](size_t index) { assert(index < m_parameters.size()); return  m_parameters[index]; }

		const RootSignatureParameter& operator[](size_t index) const { assert(index < m_parameters.size()); return  m_parameters[index]; }

		void Finalize();
	private:
		static D3D12_ROOT_SIGNATURE_FLAGS s_FullAcceseFlags;

		const wchar_t* m_name;
		ID3D12RootSignature* m_rootSignature;
		D3D12_ROOT_SIGNATURE_DESC1 m_State;
		std::vector<RootSignatureParameter> m_parameters;
		std::unique_ptr<D3D12_ROOT_PARAMETER1> m_rootParameters;
	};
}